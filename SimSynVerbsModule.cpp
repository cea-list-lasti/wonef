

#include "SimSynVerbsModule.hpp"
#include "distance.hpp"
#include "Paths.hpp"
#include "../src/tools.h"
#include <iostream>
#include <fstream>


using namespace std;

SimSynVerbsModule::SimSynVerbsModule() {
  knnStdFile =  KNNSTDFILE;
}

SimSynVerbsModule::~SimSynVerbsModule() {
}

void SimSynVerbsModule::process(WORDNET::WordNet& wn, bool /*verbose*/){ 
  for (map<string, WORDNET::WordNetEntry>::iterator itwn = wn.begin(); itwn !=wn.end(); itwn++) {
    for (map<string, WORDNET::TgtCandidates>::iterator itwne = itwn->second.frenchCandidates.begin(); itwne !=itwn->second.frenchCandidates.end(); itwne++) {
      if (itwne->second.processed=="" && itwne->second.cand.size()>0) {
	if (itwne->first.find("coupling") !=string::npos) {
	  cerr << "TRY PROCESS : " << itwne->first << " -> " << itwne->second.cand.size() << endl;
	}
	itwn->second.newdef=trySelecAndReplace(itwn->second.frenchSynset, itwn->first, itwne);
      }
    }      
  }
  for (map<string, WORDNET::WordNetEntry>::iterator itwn = wn.begin(); itwn !=wn.end(); itwn++) {
    for (map<string, WORDNET::TgtCandidates>::iterator itwne = itwn->second.frenchCandidates.begin(); itwne !=itwn->second.frenchCandidates.end(); itwne++) {
      if (itwne->second.processed=="" && itwne->second.cand.size()>0) {
	if (itwne->first.find("coupling") !=string::npos) {
	  cerr << "ENDING PROCESS : " << itwne->first << " -> " << itwne->second.cand.size() << endl;
	}	
      }
    }      
  }
}


string SimSynVerbsModule::trySelecAndReplace(map<string, set<WORDNET::TranslationInfos> >& synset,
					string synsetId,
					map<string, WORDNET::TgtCandidates>::iterator it,
					bool homograph) {

  string knnFile=boost::regex_replace(knnStdFile, boost::regex("[$]REL"), "COD_V.reverse");

  set<pair<string, float> > elected;
  pair<string, float> elec = selectTgtWord(it->second.cand, it->second.verbCand, synset, knnFile);

  if (elec.first!="") {
    elected.insert(elec);      
  }

  if (elected.size()==0) {
    knnFile=boost::regex_replace(knnStdFile, boost::regex("[$]REL"), "CPL_V.reverse");
    elec = selectTgtWord(it->second.cand, it->second.verbCand, synset, knnFile);
    if (elec.first!="") {
      elected.insert(elec);      
    }
  }

  if (elected.size()==0) {
    knnFile=boost::regex_replace(knnStdFile, boost::regex("[$]REL"), "CPLV_V.reverse");
    elec = selectTgtWord(it->second.cand, it->second.verbCand, synset, knnFile);
    if (elec.first!="") {
      elected.insert(elec);      
    }
  }
/*
  if (elected.size()==0) {
    knnFile=boost::regex_replace(knnStdFile, boost::regex("[$]REL"), "SUJ_V.reverse");
    elec = selectTgtWord(it->second.cand, synset, knnFile);
    if (elec!="") {
      elected.insert(elec);      
    }
  }
*/
  if (elected.size()!=0) {
    it->second.processed="simsyn";  
    for (set<pair<string, float> >::iterator itElec = elected.begin(); itElec != elected.end(); itElec++) {   
      WORDNET::TranslationInfos translationInfos;
      translationInfos.original = it->first;
      translationInfos.processed = "simsyn";
      translationInfos.score = itElec->second;
      synset[itElec->first].insert(translationInfos);
    }
    return LoaderVerbsModule::tgt2TgtDefs[(*elected.begin()).first];
  }

  return "";
}

pair<string, float> SimSynVerbsModule::selectTgtWord (map<string, int>& cand, map<string, string>& verbCand, map<string, set<WORDNET::TranslationInfos> >& synset, string& knnFile) {
  map<pair<string, float>, uint> votes;
  for (map<string, set<WORDNET::TranslationInfos> >::iterator itSynset = synset.begin() ; itSynset!=synset.end() ; itSynset++) {
    knnFile=boost::regex_replace(knnFile, boost::regex("[$]WORD"), itSynset->first);
    string knns;
        cerr << "Opening : " << knnFile << endl;
    ifstream knnIfs(knnFile.c_str());
    getline(knnIfs, knns);
    knnIfs.close();
    size_t bestPos = 1000;
    pair<string, float> syn;
    syn.first="";
    for (map<string,int>::iterator it2 = cand.begin(); it2!=cand.end(); it2++) {
//            cerr << "Processing : " << it2->first << " - " << verbCand[it2->first] << endl;
    
      stringstream sssearch;
      // compute the score without the pronoun
      sssearch << " "<< verbCand[it2->first]<<":";
      size_t pos = knns.find(sssearch.str());
      if (pos!=string::npos) {
	pair<string, float> candidate;
	candidate.first = it2->first;
	candidate.second = pos;
	votes[candidate] += it2->second;
	if(pos < bestPos) {
	  bestPos = pos;
	  syn = candidate;
	}
      }
    }
   // TODO:  ponderate score with proximity
    votes[syn]++;
  }
  
  size_t bestVote = 0;
  pair<string, float> elected;
  for(map<pair<string, float>, uint>::iterator itVotes= votes.begin(); itVotes!=votes.end() ; itVotes++)  {
    if (itVotes->second > bestVote && (itVotes->first).first != "") {
      bestVote = itVotes->second;
      elected = itVotes->first;
    }
  }
  return elected;
}



