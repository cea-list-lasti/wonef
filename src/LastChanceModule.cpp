#include "LastChanceModule.hpp"
#include "tools.h"



LastChanceModule::LastChanceModule(int _idModuleConf, int _nIteration, bool _verbose)
  : TranslatorModule(_verbose) {
  std::ostringstream oss;
  oss << _idModuleConf << "." << _nIteration;
  suffix = oss.str();
}

LastChanceModule::~LastChanceModule() {
}

void LastChanceModule::process(WORDNET::WordNet& wn){
  for (std::map<std::string, WORDNET::WordNetEntry>::iterator itwn = wn.begin(); itwn !=wn.end(); itwn++) {
    for (std::map<std::string, WORDNET::TgtCandidates>::iterator itwne = itwn->second.frenchCandidates.begin(); itwne !=itwn->second.frenchCandidates.end(); itwne++) {	
      if (itwne->second.cand.size()>0) {	
	itwn->second.newdef=trySelecAndReplace(itwn->second.frenchSynset, itwne);
      }
    }
  }
}




std::string LastChanceModule::trySelecAndReplace(std::map<std::string, std::set<WORDNET::TranslationInfos> >& synset,
					std::map<std::string, WORDNET::TgtCandidates>::iterator it) {
  std::string elected;


  // promote diversity
  for (std::map<std::string, int>::iterator itcand = it->second.cand.begin(); itcand != it->second.cand.end(); itcand++) {
    if (synset.find(itcand->first)!=synset.end()) {	 
      it->second.cand[itcand->first]--;
    }
  } 

 
  int bestScore =0;
  for (std::map<std::string, int>::iterator itLastChance = it->second.cand.begin(); itLastChance!=it->second.cand.end(); itLastChance++) {
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
