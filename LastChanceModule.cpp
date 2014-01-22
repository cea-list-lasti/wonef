#include "LastChanceModule.hpp"
#include "tools.h"


using namespace std;

LastChanceModule::LastChanceModule(int _idModuleConf, int _nIteration, bool _verbose)
  : TranslatorModule(_verbose) {
  std::ostringstream oss;
  oss << _idModuleConf << "." << _nIteration;
  suffix = oss.str();
}

LastChanceModule::~LastChanceModule() {
}

void LastChanceModule::process(WORDNET::WordNet& wn){
  for (map<string, WORDNET::WordNetEntry>::iterator itwn = wn.begin(); itwn !=wn.end(); itwn++) {
    for (map<string, WORDNET::TgtCandidates>::iterator itwne = itwn->second.frenchCandidates.begin(); itwne !=itwn->second.frenchCandidates.end(); itwne++) {	
      if (itwne->second.cand.size()>0) {	
	itwn->second.newdef=trySelecAndReplace(itwn->second.frenchSynset, itwne);
      }
    }
  }
}




string LastChanceModule::trySelecAndReplace(map<string, set<WORDNET::TranslationInfos> >& synset,
					map<string, WORDNET::TgtCandidates>::iterator it) {
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
    }
  }
  if (elected!="") {
    WORDNET::TranslationInfos translationInfos;
    translationInfos.original = it->first;
    translationInfos.processed = "vote-lastchance" + suffix;
    translationInfos.score = bestScore;
    synset[elected].insert(translationInfos);
    return Dictionaries::definition[elected];
  }
  return "";
}
