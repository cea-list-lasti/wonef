#include "SimSynModule.hpp"
#include "knn.pb.h"
#include "Paths.hpp"
#include "tools.h"
#include <iostream>
#include <fstream>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex/icu.hpp>
#include <list>



SimSynModule::SimSynModule(std::string _pos, int idModuleConf, int nIteration, bool _verbose) : 
  TranslatorModule(_verbose), pos(_pos) {
  std::ostringstream oss;
  oss << idModuleConf << "." << nIteration;
  suffix = oss.str();

  if (pos == "noun") {
    rels.push_back("COMPDUNOM");
    rels.push_back("COD_V");
    rels.push_back("APPOS");
  } else if (pos == "verb") {
    rels.push_back("COD_V.reverse");
    rels.push_back("CPL_V.reverse");
    rels.push_back("CPLV_V.reverse");
    //rels.push_back("SUJ_V.reverse");
  } else if (pos == "adj") {
    rels.push_back("SUBADJPOST");
    rels.push_back("ADVADJ.reverse");
    rels.push_back("ATB_SG");
  } else if (pos == "adv") {
    //rels.push_back("AdvVerbe");
    rels.push_back("AdvSub.reverse");
    rels.push_back("ADVADJ.reverse");
  }

  loadKnnDistsCache();

}

void SimSynModule::process(WORDNET::WordNet& wn){
  typedef std::pair<const std::string, WORDNET::WordNetEntry> synset_t;
  typedef std::pair<const std::string, WORDNET::TgtCandidates> candidate_t;

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

std::string SimSynModule::trySelectAndReplace(WORDNET::WordNetEntry& synset,
  std::pair<std::string, WORDNET::TgtCandidates> candidate) {

  unsigned int i = 0;
  size_t maximumValue = 0;
  // thresholds learnt using WOLF manually validated data
  if (pos == "verb") {
    maximumValue = 7704;
  } else if (pos == "noun") {
    maximumValue = 7600;
  } else if (pos == "adj") {
    maximumValue = 4710;
  } else if (pos == "adv") {
    maximumValue = 6000;
  }

  std::set<std::pair<std::string, size_t> > elected;

  while ((elected.empty() || elected.begin()->second >= maximumValue) && i < rels.size()) {
      std::pair<std::string, size_t> elec = selectTgtWord(candidate.second.cand, candidate.second.verbCand, synset.frenchSynset, rels[i]);
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
    for (std::set<std::pair<std::string, size_t> >::iterator itElec = elected.begin(); itElec != elected.end(); itElec++) {
      WORDNET::TranslationInfos translationInfos;
      translationInfos.original = candidate.first;
      translationInfos.processed = "simsyn" + suffix;
      translationInfos.score = itElec->second;
      if (translationInfos.score < maximumValue) {
        synset.frenchSynset[itElec->first].insert(translationInfos);
      }
      synset.frenchScore[itElec->first].insert(translationInfos);
    }
    return Dictionaries::definition[(*elected.begin()).first];
  }
  return "";
}

boost::optional<int> SimSynModule::getDistance(const std::string &relation, const std::string& word, const std::string& related) {
  std::map<std::string, std::map<std::string, int> >& relationCache = knnDistsCache[relation];

  if (relationCache.find(word) == relationCache.end()
      || relationCache[word].find(related) == relationCache[word].end()) {
    return boost::optional<int>();
  } else {
    // restore the value from the cache
    return boost::optional<int>(relationCache[word][related]);
  }
}

std::pair<std::string, size_t> SimSynModule::selectTgtWord (std::map<std::string, int>& cand, std::map<std::string, std::string>& verbCand, std::map<std::string, std::set<WORDNET::TranslationInfos> >& synset, const std::string& relation) {
  std::map<std::pair<std::string, size_t>, uint> votes;
  for (std::map<std::string, std::set<WORDNET::TranslationInfos> >::iterator itSynset = synset.begin(); itSynset != synset.end(); itSynset++) {

    int bestDist = INT_MAX;
    std::pair<std::string, size_t> syn;
    syn.first = "";
    for (std::map<std::string,int>::iterator it2 = cand.begin(); it2!=cand.end(); it2++) {
      /* Look at the position of our candidate in the synonyms list */
      std::string possiblyRelated = pos == "verb" ? verbCand[it2->first] : it2->first;

      boost::optional<int> maybe_semantic_distance = getDistance(relation, itSynset->first, possiblyRelated);
      if (maybe_semantic_distance) {
        int semantic_distance = *maybe_semantic_distance;
        std::pair<std::string, float> candidate;
        candidate.first = it2->first;
        candidate.second = semantic_distance;
        votes[candidate] += it2->second;
        if(semantic_distance < bestDist && semantic_distance > 0) {
          bestDist = semantic_distance;
          syn = candidate;
        }
      }
    }
    // TODO:  ponderate score with proximity
    votes[syn]++;
  }

  size_t bestVote = 0;
  std::pair<std::string, size_t> elected;
  for(std::map<std::pair<std::string, size_t>, uint>::iterator itVotes = votes.begin(); itVotes != votes.end(); itVotes++)  {
    if (itVotes->second > bestVote && (itVotes->first).first != "") {
      bestVote = itVotes->second;
      elected = itVotes->first;
    }
  }
  return elected;
}



// precondition: protofile contains the correct data
void SimSynModule::readProtobuf(std::string relation) {
  std::string protofile = KNNPROTOFILE + pos + "_" + relation;

  KnnDists knnDistsFile;
  std::fstream in(protofile.c_str(), std::ios::in | std::ios::binary);
  if (!knnDistsFile.ParseFromIstream(&in)) {
    std::cerr << "Failed to parse " << protofile << std::endl;
    exit(1);
  }

  std::map<std::string, std::map<std::string, int> >& relationCache = knnDistsCache[relation];

  assert(relation == knnDistsFile.relation());
  for (int i = 0; i < knnDistsFile.word_size(); i++) {
    KnnDists::Word w = knnDistsFile.word(i);

    assert(w.related_size() == w.distance_size());
    for (int j = 0; j < w.related_size(); j++) {
      relationCache[w.word()][w.related(j)] = w.distance(j);
    }
  }
}

void SimSynModule::loadKnnDistsCache() {
  BOOST_FOREACH(std::string &relation, rels) {
    std::string protofile = KNNPROTOFILE + pos + "_" + relation;
    if (std::ifstream(protofile.c_str(), std::ios::in).good()) {
      readProtobuf(relation);
    } else {
      std::cerr << "Error: protobuf file not found: " << protofile << "." << std::endl;
    }
  }
}
