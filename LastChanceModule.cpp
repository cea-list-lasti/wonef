#include "LastChanceModule.hpp"
#include "distance.hpp"
#include "../src/tools.h"


using namespace std;

LastChanceModule::LastChanceModule() {
}

LastChanceModule::~LastChanceModule() {
}

void LastChanceModule::process(WORDNET::WordNet& wn, bool /*verbose*/){
  for (map<string, WORDNET::WordNetEntry>::iterator itwn = wn.begin(); itwn !=wn.end(); itwn++) {
    for (map<string, WORDNET::TgtCandidates>::iterator itwne = itwn->second.frenchCandidates.begin(); itwne !=itwn->second.frenchCandidates.end(); itwne++) {	
      if (itwne->second.processed=="" && itwne->second.cand.size()>0) {	
	itwn->second.newdef=trySelecAndReplace(itwn->second.frenchSynset, itwn->first, itwne, false);
      }
    }
  }
}




string LastChanceModule::trySelecAndReplace(map<string, set<pair<string, float> > >& synset,
					string synsetId,
					map<string, WORDNET::TgtCandidates>::iterator it,
					bool homograph) {
  string elected;


  // promote diversity
  for (map<string, int>::iterator itcand = it->second.cand.begin(); itcand != it->second.cand.end(); itcand++) {
    if (synset.find(itcand->first)!=synset.end()) {	 
      it->second.cand[itcand->first]--;
    }
  } 

 
  int bestScore =0;
  for (map<string, int>::iterator itLastChance = it->second.cand.begin(); itLastChance!=it->second.cand.end(); itLastChance++) {
    if (itLastChance->second >=bestScore && itLastChance->second!=0) {
      bestScore = itLastChance->second;
      elected=itLastChance->first;	
      it->second.processed="vote-lastchance";
    }
  }
  if (elected!="") {
    pair<string, float> score;
    score.first = it->first;
    score.second = bestScore;
    synset[elected].insert(score);
    return LoaderModule::tgt2TgtDefs[elected];
  }
  return "";
}
