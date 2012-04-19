

#include "SimSynModule.hpp"
#include "distance.hpp"
#include "Paths.hpp"
#include "../src/tools.h"
#include <iostream>
#include <fstream>


using namespace std;


SimSynModule::SimSynModule() {
  knnStdFile =  KNNSTDFILE;
}


SimSynModule::~SimSynModule() {
}


void SimSynModule::process(WORDNET::WordNet& wn, bool verbose){ 
  if (wn["00472185"].frenchCandidates.size()!=0) {
    cerr << "TEST3 : " << wn["00472185"].frenchCandidates["coupling"].cand.size() << endl;
  }
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



string SimSynModule::trySelecAndReplace(map<string, set<string> >& synset,
					string synsetId,
					map<string, WORDNET::TgtCandidates>::iterator it,
					bool homograph) {
  string knnFile=boost::regex_replace(knnStdFile, boost::regex("[$]REL"), "COMPDUNOM");
  set<string> elected;
  string elec = selectTgtWord(it->second.cand, synset, knnFile);
  if (elec!="") {
    elected.insert(elec);      
  }
  if (elected.size()==0) {
    knnFile=boost::regex_replace(knnStdFile, boost::regex("[$]REL"), "COD_V");
    elec = selectTgtWord(it->second.cand, synset, knnFile);
    if (elec!="") {
      elected.insert(elec);      
    }
  }
  if (elected.size()==0) {
    knnFile=boost::regex_replace(knnStdFile, boost::regex("[$]REL"), "APPOS");
    elec = selectTgtWord(it->second.cand, synset, knnFile);
    if (elec!="") {
      elected.insert(elec);      
    }
  }

  if (elected.size()!=0) {
    it->second.processed="simsyn";  
    for (set<string>::iterator itElec = elected.begin(); itElec != elected.end(); itElec++) {      
      synset[*itElec].insert(it->first);
    }
    return LoaderModule::tgt2TgtDefs[*elected.begin()];
  }
  return "";
}


string SimSynModule::selectTgtWord (map<string, int>& cand, map<string, set<string> >& synset, string& knnFile) {
  map<string, uint> votes;
  for (map<string, set<string> >::iterator itSynset = synset.begin() ; itSynset!=synset.end() ; itSynset++) {
    knnFile=boost::regex_replace(knnFile, boost::regex("[$]WORD"), itSynset->first);
    string knns;
    //    cerr << "Opening : " << knnFile << endl;
    ifstream knnIfs(knnFile.c_str());
    getline(knnIfs, knns);
    knnIfs.close();
    int bestPos = 1000;
    string syn = "";
    for (map<string,int>::iterator it2 = cand.begin(); it2!=cand.end(); it2++) {
      //      cerr << "Processing : " << it2->first << endl;
    
      stringstream sssearch;
      sssearch << " "<< it2->first<<":";
      int pos = knns.find(sssearch.str());
      if (pos!=string::npos) {
        votes[it2->first]+=it2->second;
	if( pos < bestPos) {
	  bestPos=pos;
	  syn=it2->first;
	}
      }	    
    }
   // TODO:  ponderate score with proximity
    votes[syn]++;
  }
  
  int bestVote = 0;
  string elected = "";
  for(map<string, uint>::iterator itVotes= votes.begin(); itVotes!=votes.end() ; itVotes++)  {
    if (itVotes->second> bestVote && itVotes->first !="" ) {
      bestVote = itVotes->second;
      elected = itVotes->first;
    }
  }
  return elected;
}



