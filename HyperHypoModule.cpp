#include "HyperHypoModule.hpp"
#include "Loader.hpp"
#include "distance.hpp"
#include "Paths.hpp"
#include "../src/tools.h"
#include <iostream>
#include <fstream>
#include "boost/regex.hpp"

using namespace std;


HyperHypoModule::HyperHypoModule(string dataInput, string typeroler, TRMode _mode)
    : mode(_mode) {
  if (typeroler.compare("COMPDUNOM")==0) {
    tRoler = TypeRoler(TYPEROLERFILE);
  } else if (typeroler.compare("SUJ_V")==0) {
    tRoler = TypeRoler(TYPEROLERFILE2);
  } else if (typeroler.compare("COD_V")==0) {
    tRoler = TypeRoler(TYPEROLERFILE3);
  }else if (typeroler.compare("window10")==0) {
    tRoler = TypeRoler(TYPEROLERFILE4);
  }
  loadHyperHypos(dataInput);  
}


HyperHypoModule::~HyperHypoModule() {
  cerr << "Deleting hypernyms " << endl;
  cerr << "Deleting hyponyms " << endl;
  hypernyms.clear();
  hyponyms.clear();
}


void HyperHypoModule::loadHyperHypos(string dataInput) {
  ifstream dataIfs(dataInput.c_str(), fstream::in);
  if (dataIfs.fail()) {
    cerr << "Oops, " << dataInput << " doesn't exist. " << __FILE__ << ":" << __LINE__ << endl;
    exit(-1);
  }
  string s;
  int cntHypers = 0;
  int cntHypos = 0;
  while (getline(dataIfs, s) ) {	
    string str = s;
    string synsetId = s.substr(0, s.find(' '));    

    std::string::const_iterator start, end; 
    boost::match_results<std::string::const_iterator> what; 
    boost::match_flag_type flags = boost::match_default; 

    // extract hypernym && hyponyms
    start = s.begin(); 
    end = s.end(); 
    while(regex_search(start, end, what, boost::regex("@ ([^ ]*) "), flags))  { 
      hypernyms[synsetId].insert(string(what[1].first, what[1].second));
      // update search position: 
      start = what[1].second; 
      // update flags: 
      flags |= boost::match_prev_avail; 
      flags |= boost::match_not_bob; 
    }
    cntHypers+=hypernyms[synsetId].size();

    while(regex_search(start, end, what, boost::regex("~ ([^ ]*) "), flags))  { 
      hyponyms[synsetId].insert(string(what[1].first, what[1].second)); 
      // update search position: 
      start = what[1].second; 
      // update flags: 
      flags |= boost::match_prev_avail; 
      flags |= boost::match_not_bob; 
    }
    cntHypos+=hyponyms[synsetId].size();
  }

  dataIfs.close();  

  
  cerr << "Total Hyperonyms : " << cntHypers << endl;
  cerr << "Total Hyponyms : " << cntHypos << endl;  
}



void HyperHypoModule::process(WORDNET::WordNet& wn, bool verbose ){
  map<string, set<string> >reverseIndex;
  
  int nbDisamb = 0;

  for (map<string, WORDNET::WordNetEntry>::iterator itwn = wn.begin(); itwn !=wn.end(); itwn++) {
    for (map<string, set<WORDNET::TranslationInfos> >::iterator itwne = itwn->second.frenchSynset.begin(); itwne !=itwn->second.frenchSynset.end(); itwne++) {	
      reverseIndex[itwn->first].insert(itwne->first); 	
    }
  }
  cerr << "Reverse Index size : " << reverseIndex.size() << endl;
  for (map<string, WORDNET::WordNetEntry>::iterator itwn = wn.begin(); itwn !=wn.end(); itwn++) {    
    for (map<string, WORDNET::TgtCandidates >::iterator it = itwn->second.frenchCandidates.begin(); it != itwn->second.frenchCandidates.end(); it++) {
      float best = 0;
      string elected = "";
      for (map<string, int>::iterator itCand = it->second.cand.begin(); itCand != it->second.cand.end(); itCand++) {
	int validSumHypo = 0;
	int validSumHyper = 0;
	float sum = 0;
	string head = "";
/*	if(hyponyms[itwn->first].size()==0) {
		validSumHypo+=2;
	}
	*/
	for(set<string>::iterator itHypos = hyponyms[itwn->first].begin(); itHypos != hyponyms[itwn->first].end(); itHypos++) { 
	  for (set<string>::iterator itSyn = reverseIndex[*itHypos].begin(); itSyn !=  reverseIndex[*itHypos].end(); itSyn++) {
	    validSumHypo++;	
	    itwn->second.hypos.insert(*itSyn);
	    if (head=="") {
	    	head = getHead(*itSyn);
	    }
	    float score = tRoler.computeIsAScore( itCand->first, head, mode);

	    if( verbose) {
//	      cerr << "DEBUG "<<" : " << it->first << " : " << itCand->first << " > " << *itSyn << " : " << score << endl;
	    }
	    if (!isnan(score)) {
	      if( verbose) {
		cerr << "DEBUG hyponyms"<<" : " << it->first << " : " << itCand->first << " > " << *itSyn << " : " << score << endl;
	      }
	      sum+=score==1?0.3:score;
	    } else {
	      validSumHypo --;
	    } 
	    if (score==0 && head[head.length()-1]=='s') {
	    	head=head.substr(0, head.length()-1);
	    	itSyn--;
		validSumHypo --;
	    } else {
	    	head="";
	    }
	  }
	}
/*	if (hypernyms[itwn->first].size()==0) {
		validSumHyper+=2;
	}
	*/
	for(set<string>::iterator itHyper = hypernyms[itwn->first].begin(); itHyper != hypernyms[itwn->first].end(); itHyper++) { 
	  for (set<string>::iterator itSyn = reverseIndex[*itHyper].begin(); itSyn !=  reverseIndex[*itHyper].end(); itSyn++) {	  	
	    validSumHyper++;
	    itwn->second.hypers.insert(*itSyn);
	    if (head=="") {
		    head = getHead(*itSyn);
	    }
	    float score =tRoler.computeIsAScore(head, itCand->first, mode);
	    if (!isnan(score)) {
	         cerr << "DEBUG hypernyms : " << it->first << " : " << itCand->first << " < " << *itSyn << " : " << score << endl;
	      sum+=score==1?0.3:score;
	    } else {
	      validSumHyper --;
	    }
	    if (score == 0 && head[head.length()-1]=='s') {
	    	head=head.substr(0, head.length()-1);
	    	itSyn--;
		validSumHyper --;
	    } else {
	    	head="";
	    }
	  }
	}
	/*
	Distance lDist;
	if (lDist.LD(desax(LoaderModule::desaxData, itCand->first),it->first)<=3) {
	  sum*=1.+ (1.-0.2*lDist.LD(desax(LoaderModule::desaxData, itCand->first),it->first));
	}
	*/
	sum =  sum / (validSumHypo + validSumHyper);
	if (verbose && sum >0 ) {
		cerr << itwn->first<<":"<<it->first << " -> " << itCand->first << ":" << sum << endl;
	}
	if (validSumHypo>1 && validSumHyper > 1 && sum> best) {
	  best = sum;
	  elected = itCand->first;
	}
	
      }
     /* 	if (validSumHypo<2 || validSumHypo<2) {
		elected="";
	}
	*/
	cerr << "elected :"<< elected << endl;

      if (elected!="") {
	if (itwn->second.frenchSynset.find(elected)==itwn->second.frenchSynset.end()) {
	  itwn->second.frenchSynset[elected] = set<WORDNET::TranslationInfos>();
	}
	WORDNET::TranslationInfos translationInfo;
	translationInfo.original = it->first;
	translationInfo.processed = "hyperhypo";
	translationInfo.score = best;
	itwn->second.frenchSynset[elected].insert(translationInfo);
	itwn->second.newdef=LoaderModule::tgt2TgtDefs[elected];
	nbDisamb++;
      }
    }
  


  }

  cout << "Nb disamb : " << nbDisamb << endl;



}


  


string HyperHypoModule::getHead (string locution) {
  string endLoc = locution;
  if (LoaderModule::nounsList.find(locution)==LoaderModule::nounsList.end()) {
    return locution;
  }

  string partialLoc = endLoc.substr(0, endLoc.find('_'));
  while (LoaderModule::nounsList.find(partialLoc)==LoaderModule::nounsList.end()
	 && partialLoc!=endLoc 
	 && partialLoc!=endLoc.substr(0, endLoc.length()-1)) {
    endLoc = endLoc.substr(endLoc.find('_')+1);
    partialLoc = endLoc.substr(0, endLoc.find('_'));
    if (partialLoc[partialLoc.length()-1]=='s'
	||partialLoc[partialLoc.length()-1]=='x') {
      if (LoaderModule::nounsList.find(partialLoc)==LoaderModule::nounsList.end()) {
	partialLoc=partialLoc.substr(0, partialLoc.length()-1);
      }
    }
  }
  string head =partialLoc ;
  
  return head;	  
}
