#include "SimSynModule.hpp"
#include "distance.hpp"
#include "Paths.hpp"
#include "../src/tools.h"
#include <iostream>
#include <fstream>
#include <boost/foreach.hpp>

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)-1)
#endif

using namespace std;


SimSynModule::SimSynModule(string _pos, int idModuleConf, int nIteration) : pos(_pos) {
  knnStdFile =  KNNSTDFILE;
  std::ostringstream oss;
  oss << idModuleConf << "." << nIteration;
  suffix = oss.str();
}

void SimSynModule::process(WORDNET::WordNet& wn, bool /*verbose*/){
  typedef std::pair<const string, WORDNET::WordNetEntry> synset_t;
  typedef std::pair<const string, WORDNET::TgtCandidates> candidate_t;

  BOOST_FOREACH(synset_t& synset, wn) {
    /* For every potential translation in our synset, look for already
     * translated synonyms */
    BOOST_FOREACH(candidate_t& candidate, synset.second.frenchCandidates) {
      if (candidate.second.cand.size() > 0) {
        synset.second.newdef = trySelectAndReplace(synset.second, candidate);
      }
    }
  }
}

string SimSynModule::trySelectAndReplace(WORDNET::WordNetEntry& synset,
  pair<string, WORDNET::TgtCandidates> candidate) {

 std::vector<string> rels;
  if (pos == "noun") {
    rels.push_back("COMPDUNOM");
    rels.push_back("COD_V");
    rels.push_back("APPOS");
  } else if (pos == "verb") {
    rels.push_back("COD_V.reverse");
    rels.push_back("CPL_V.reverse");
    rels.push_back("CPLV_V.reverse");
  }

  unsigned int i = 0;
  size_t electionThreshold = 1000;
  set<pair<string, size_t> > elected;

  while ((elected.empty() || elected.begin()->second >= electionThreshold) && i < rels.size()) {
    string knnFile=boost::regex_replace(knnStdFile, boost::regex("[$]REL"), rels[i]);
    pair<string, size_t> elec = selectTgtWord(candidate.second.cand, candidate.second.verbCand, synset.frenchSynset, knnFile);
    if (elec.first != "") {
      // First time, "elected" is empty.
      // Then, we keep only the "elec" with shortest distance.
      if (elected.empty() || (!elected.empty() && elec.second < elected.begin()->second)) {
        elected.clear();
        elected.insert(elec);
      }
    }
    i++;
  }

  if (!elected.empty()) {
    for (set<pair<string, size_t> >::iterator itElec = elected.begin(); itElec != elected.end(); itElec++) {
      WORDNET::TranslationInfos translationInfos;
      translationInfos.original = candidate.first;
      translationInfos.processed = "simsyn" + suffix;
      translationInfos.score = itElec->second;
      if (translationInfos.score < electionThreshold) {
        synset.frenchSynset[itElec->first].insert(translationInfos);
      }
      synset.frenchScore[itElec->first].insert(translationInfos);
    }
    return LoaderModule::tgt2TgtDefs[(*elected.begin()).first];
  }
  return "";
}


pair<string, size_t> SimSynModule::selectTgtWord (map<string, int>& cand, map<string, string>& verbCand, map<string, set<WORDNET::TranslationInfos> >& synset, const string& knnRelFile) {
  map<pair<string, size_t>, uint> votes;
  for (map<string, set<WORDNET::TranslationInfos> >::iterator itSynset = synset.begin(); itSynset != synset.end(); itSynset++) {

    string knnFile=boost::regex_replace(knnRelFile, boost::regex("[$]WORD"), itSynset->first);
    //cerr << "Opening : " << knnFile << endl;
    string knns;
    ifstream knnIfs(knnFile.c_str());
    getline(knnIfs, knns);
    knnIfs.close();

    size_t bestDist = SIZE_MAX;
    pair<string, size_t> syn;
    syn.first = "";
    for (map<string,int>::iterator it2 = cand.begin(); it2!=cand.end(); it2++) {
      /* Look at the position of our candidate in the synonyms list */
      stringstream sssearch;
      if (pos == "noun") {
        sssearch << " " << it2->first << ":";
      } else if (pos == "verb") {
        // compute the score without the pronoun
        sssearch << " " << verbCand[it2->first] << ":";
      }

      size_t dist = knns.find(sssearch.str());
      if (dist!=string::npos) {
        pair<string, float> candidate;
        candidate.first = it2->first;
        candidate.second = dist;
        votes[candidate] += it2->second;
        if(dist < bestDist && dist > 0) {
          bestDist = dist;
          syn = candidate;
        }
      }
    }
   // TODO:  ponderate score with proximity
    votes[syn]++;
  }

  size_t bestVote = 0;
  pair<string, size_t> elected;
  for(map<pair<string, size_t>, uint>::iterator itVotes = votes.begin(); itVotes != votes.end(); itVotes++)  {
    if (itVotes->second > bestVote && (itVotes->first).first != "") {
      bestVote = itVotes->second;
      elected = itVotes->first;
    }
  }
  return elected;
}

