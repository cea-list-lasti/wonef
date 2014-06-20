#include "MeroHoloLikeHyperModule.hpp"
#include "Paths.hpp"
#include "tools.h"
#include <iostream>
#include <fstream>


using namespace std;


MeroHoloLikeHyperModule::MeroHoloLikeHyperModule(string& datafile, int idModuleConf, int nIteration)
  : MeroHoloModule(datafile, idModuleConf, nIteration) {

  tRoler = TypeRoler(TYPEROLERFILE, "COMPDUNOM");

}


MeroHoloLikeHyperModule::~MeroHoloLikeHyperModule() {
}


void MeroHoloLikeHyperModule::finalize() {
  meronyms.clear();
  holonyms.clear();
}






void MeroHoloLikeHyperModule::process(WORDNET::WordNet& wn, TRMode mode, bool verbose ){
  int nbDisamb = 0;
  
  for (map<string, WORDNET::WordNetEntry>::iterator itwn = wn.begin(); itwn !=wn.end(); itwn++) {
    for (map<string, set<WORDNET::TranslationInfos> >::iterator itwne = itwn->second.frenchSynset.begin(); itwne !=itwn->second.frenchSynset.end(); itwne++) {	
      reverseIndex[itwn->first].insert(itwne->first); 	
    }
  }
  for (map<string, WORDNET::WordNetEntry>::iterator itwn = wn.begin(); itwn !=wn.end(); itwn++) {    
    for (map<string, WORDNET::TgtCandidates >::iterator it = itwn->second.frenchCandidates.begin(); it != itwn->second.frenchCandidates.end(); it++) {
      float best = 0;
      pair<string, float> elected;
      for (map<string, int>::iterator itCand = it->second.cand.begin(); itCand != it->second.cand.end(); itCand++) {
	float sum = 0;
	int nbMeroHolo = 0;
	for(set<string>::iterator itHypos = meronyms[itwn->first].begin(); itHypos != meronyms[itwn->first].end(); itHypos++) { 
	  for (set<string>::iterator itSyn = reverseIndex[*itHypos].begin(); itSyn !=  reverseIndex[*itHypos].end(); itSyn++) {
	    itwn->second.hypos.insert(*itSyn);
	    string head = HyperHypoModule::getHead(*itSyn);
	    float score = tRoler.computeIsAScore( itCand->first, head, mode);

	    if (!isnan(score)) {
	      if( verbose) {
	        cerr << "DEBUG MERO "<<" : " << it->first << " : " << itCand->first << " > " << *itSyn << " : " << score << endl;
	      }
	      sum+=score;
	      nbMeroHolo ++;
	    }
	  }
	}
	for(set<string>::iterator itHyper = holonyms[itwn->first].begin(); itHyper != holonyms[itwn->first].end(); itHyper++) { 
	  for (set<string>::iterator itSyn = reverseIndex[*itHyper].begin(); itSyn !=  reverseIndex[*itHyper].end(); itSyn++) {
	    itwn->second.hypers.insert(*itSyn);
	    string head = HyperHypoModule::getHead(*itSyn);
	    float score =tRoler.computeIsAScore(head, itCand->first, mode);
	    if (!isnan(score)) {
	      if( verbose) {
	        cerr << "DEBUG HOLO "<<" : " << it->first << " : " << itCand->first << " < " << *itSyn << " : " << score << endl;
	    }
	      sum+=score;
	      nbMeroHolo ++;
	    }
	  }
	}
	/*
	if (levenshtein(desaxUTF8(itCand->first),it->first)<=3) {
	  sum*=1.+ (1.-0.2*lDist.LD(desaxUTF8(itCand->first),it->first));
	}
	*/
	sum = sum / nbMeroHolo;

	if (sum> best) {
	  best = sum;
	  elected.first = itCand->first;
	  elected.second = best;
	}

      }
      if (elected.first != "") {
	if (itwn->second.frenchSynset.find(elected.first)==itwn->second.frenchSynset.end()) {
	  itwn->second.frenchSynset[elected.first]=set<WORDNET::TranslationInfos>();
	}
	WORDNET::TranslationInfos translationInfos;
	translationInfos.original = it->first;
	translationInfos.processed = "hyperlike" + suffix;
	translationInfos.score = elected.second;
	itwn->second.frenchSynset[elected.first].insert(translationInfos);
	itwn->second.newdef=Dictionaries::definition[elected.first];
	if (verbose) {
	  cerr << "ELECTED : " << elected.first << " score : " << translationInfos.score << endl;
	}
	nbDisamb++;
      }
    }
  


  }

}


