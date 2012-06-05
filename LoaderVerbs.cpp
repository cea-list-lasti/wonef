#include "LoaderVerbs.hpp"
#include "distance.hpp"
#include "Tools.hpp"
#include "../src/tools.h"
#include <iostream>
#include "boost/regex.hpp"

using namespace std;

set<string> LoaderVerbsModule::verbsList;
map<string, string> LoaderVerbsModule::desaxData;
map<string, string> LoaderVerbsModule::tgt2TgtDefs;
WORDNET::WordNetIndex LoaderVerbsModule::WNIndex;


LoaderVerbsModule::LoaderVerbsModule(string _infile, set<string>& _dicfiles, string verbsfile) {
  infile = _infile;
  dicfiles = _dicfiles;
  cerr << "Loading...  " << endl;
  
  if(desaxData.size()==0) {
    initDesax(desaxData) ;
    LoaderVerbsModule::loadPOSList(verbsfile);
    LoaderVerbsModule::loadBilingualDic();
    LoaderVerbsModule::loadIndex();
  }
}


LoaderVerbsModule::~LoaderVerbsModule() {
}


void LoaderVerbsModule::loadIndex() {
  string indexFile = ""; 
  if (infile.find("data.") !=string::npos) {
    indexFile = infile.replace(infile.rfind("data"), 4,"index");
  } else if (infile.find("data") !=string::npos) {
    indexFile = infile.replace(infile.rfind("DAT"), 3,"IDX");
  }

  cerr << "Opening "<< indexFile << endl;
  ifstream idss(indexFile.c_str());
  string s;
  while (getline(idss, s) ) {
    string literal = s.substr(0,s.find(' '));
    string idPattern =  " ([0-9]{8}) ";
    std::string::const_iterator start, end;
    start = s.begin();
    end = s.end();
    boost::match_results<std::string::const_iterator> what;
    boost::match_flag_type flags = boost::match_default;
    while(regex_search(start, end, what, boost::regex(idPattern), flags))  {
      boost::match_results<std::string::const_iterator> whatInside;
      start = what[1].second;
      //      end = what[0].second;
      WNIndex[literal].insert(string(what[1].first, what[1].second));
      //      cerr << "LIT : " << literal << " -> " << string(what[1].first, what[1].second) << endl;
      // update flags:
      flags |= boost::match_prev_avail;
      flags |= boost::match_not_bob;
    }
  }
  idss.close();
  cerr << "Index loaded" << endl;
}


void LoaderVerbsModule::loadPOSList(string verbsFile) {
  cerr << "Opening "<< verbsFile << endl;
  ifstream idss(verbsFile.c_str());
  string s;
  while (getline(idss, s) ) {	
    verbsList.insert(s);
  }
  idss.close();
}


void LoaderVerbsModule::loadBilingualDic() {

  for (set<string>::iterator itDic = dicfiles.begin(); itDic!=dicfiles.end(); itDic++) {
    cerr << "Opening "<< *itDic << endl;
    ifstream idss(itDic->c_str());
    string s;
    while (getline(idss, s) ) {
      s=boost::regex_replace(s, boost::regex(" "), "_");
      stringstream ss;    
      ss << s; 
      string tgtWord = s.substr(0, s.find(';'));
      ss.ignore(256,';');
      ss.ignore(256,';');
      ss.ignore(256,';');
      ss >> s;     
      //      cerr << s << endl;
      if (LoaderVerbsModule::verbsList.find(tgtWord)!=LoaderVerbsModule::verbsList.end()
	  || LoaderVerbsModule::verbsList.find(tgtWord.substr(0, tgtWord.find(' ')))!=LoaderVerbsModule::verbsList.end()
	  || (tgtWord[tgtWord.length()-1]=='s' && LoaderVerbsModule::verbsList.find(tgtWord.substr(0, tgtWord.length()-1))!=LoaderVerbsModule::verbsList.end())
	  ) {
	src2Tgt[tolower(s.substr(0, s.find(';')))].insert(tgtWord);
//cerr << "src2Tgt : " << tolower(s.substr(0, s.find(';'))) << " -> " << tgtWord << endl;
	
	if (s.find(";")+5< s.rfind(";")) {
	  tgt2TgtDefs[tgtWord]=s.substr(s.find(';')+1);
	}
      }
    }
    idss.close();
  }
}


WORDNET::TgtCandidates LoaderVerbsModule::extractCandidates(string srcWord) {
  WORDNET::TgtCandidates res;
  if (srcWord.length()<=2) {
    return res;
  }

  srcWord = tolower(srcWord);
  for (set<string>::iterator it = src2Tgt[srcWord].begin() ; it!= src2Tgt[srcWord].end(); it++) {
    string tgtWord = *it;
    res.cand[tgtWord]=0;

    // removes the pronoun in order to compute subsequent scores
    if (tgtWord.find("se_")!=string::npos) {
      res.verbCand[tgtWord] = tgtWord.substr(tgtWord.find("_")+1);
    } else if (tgtWord.find("s'")!=string::npos) {
      res.verbCand[tgtWord] = tgtWord.substr(tgtWord.find("'")+1);
    } else {
      res.verbCand[tgtWord] = tgtWord;
    }
  } 
  return res;
}


WORDNET::WordNet LoaderVerbsModule::load(bool verbose, int notmore) {
  WORDNET::WordNet wn;
  if (infile.find("index")!=string::npos) {  
    infile.replace(infile.rfind("index"), 5,"data");
  } else if (infile.find("IDX")!=string::npos) {  
    infile.replace(infile.rfind("IDX"), 3,"DAT");
  }
  ifstream dataIfs(infile.c_str());
  int cnt = 0;
  string s = "";
  cerr << infile << endl;
  cerr << "cnt : " << cnt <<"/"<< notmore << endl;

  while (getline(dataIfs, s)  && (cnt < notmore || notmore==-1)) {
    WORDNET::WordNetEntry wne;

    if (s[0]!=' ') {
      string str = s;
      string synsetId = s.substr(0, s.find(' '));
      stringstream ss;
      ss << s;
      ss.ignore(256, 'v') ;
      int nbSyns = 0xaa;
      ss >> hex >> nbSyns ; 
      for (int i = 0; i < nbSyns; i++) {
	ss.ignore(1, ' ') ;
	string srcWord;
	ss >> srcWord;
	srcWord=srcWord.substr(0, srcWord.find(' '));
	WORDNET::TgtCandidates candidates = extractCandidates(srcWord);	

	bool capital = false;
	if (WNIndex[srcWord].size()==0) {
	  srcWord = tolower(srcWord);
	  capital=true;
	  cerr << "WARNING : "<<srcWord<<" has no id" << endl;
	}

	else if (WNIndex[srcWord].size()==1) {
	  candidates.processed="monosemous";
	  //	  wne.frenchCandidates.insert(pair<string, WORDNET::TgtCandidates>(srcWord, candidates));
	  wne.frenchCandidates[srcWord]= candidates;
	  
	  for (std::map<std::string, int>::iterator it = candidates.cand.begin(); it != candidates.cand.end(); it++) {
	    if (wne.frenchSynset.find(it->first)==wne.frenchSynset.end()) {
	      wne.frenchSynset[it->first]=set<string>();  
	    }
	    wne.frenchSynset[it->first].insert(srcWord);
//	    cerr << "cand :" << it->first << " / srcWord : " << srcWord << endl;
	    //wne.frenchSynset.insert(pair<string, string>(it->first, srcWord));
	    wne.newdef=tgt2TgtDefs[it->first];
//	    cerr << "newdef : " << wne.newdef << endl;
	  }
	}
	
	else {
	  string longest = "";
	  switch (candidates.cand.size()) {
	  case 0 : 
	    candidates.processed="notranslation";
	    wne.frenchCandidates[srcWord]= candidates;
	    //	    wne.frenchCandidates.insert(pair<string, WORDNET::TgtCandidates>(srcWord, candidates));
	    if (capital) {
	      if (wne.frenchSynset.find(srcWord)==wne.frenchSynset.end()) {
		wne.frenchSynset[srcWord]=set<string>();  
	      }
	      wne.frenchSynset[srcWord].insert(srcWord);
	      //wne.frenchSynset.insert(pair<string, string>(srcWord, srcWord));
	    }
	    wne.newdef=tgt2TgtDefs[srcWord];
	    if (verbose) {
	      cerr << "NEWDEF : " << tgt2TgtDefs[tolower(srcWord)] << endl;
	    }
	    break;
	  case 1 : 
	    candidates.processed="uniq";
	    wne.frenchCandidates[srcWord]=candidates;
	    /*
	    if (srcWord.length() <=2 ) {
	      wne.frenchCandidates[srcWord].cand.clear();
	    }
	    */
	    if (wne.frenchSynset.find(candidates.cand.begin()->first)==wne.frenchSynset.end()) {
	      wne.frenchSynset[candidates.cand.begin()->first]=set<string>();  
	    }
	    wne.frenchSynset[candidates.cand.begin()->first].insert(srcWord);
	    //wne.frenchCandidates.insert(pair<string, WORDNET::TgtCandidates>(srcWord, candidates));
	    //	      wne.frenchSynset.insert(pair<string, string>(candidates.cand.begin()->first, srcWord));
	     

	    wne.newdef=tgt2TgtDefs[candidates.cand.begin()->first];
	    if (verbose) {
	      cerr << "NEWDEF : " << tgt2TgtDefs[tolower(candidates.cand.begin()->first)] << endl;
	    }
	    break;
	  case 2 : 

	    wne.frenchCandidates[srcWord]= candidates;
	    /*
	    if (srcWord.length() <=2 ) {
	      wne.frenchCandidates[srcWord].cand.clear();
	    }
	    */
	    //	      wne.frenchCandidates.insert(pair<string, WORDNET::TgtCandidates>(srcWord, candidates));	    
	  
	    // promote more specific terms
	    if (candidates.cand.begin()->first.find(candidates.cand.rbegin()->first)!=string::npos 
		&& ((*candidates.cand.rbegin()).first.length() - (*candidates.cand.begin()).first.length() > 2 )) {
	      wne.frenchCandidates[srcWord].cand[candidates.cand.begin()->first]++;
	      if (verbose) {
		cerr << srcWord << "\t *** ";
		cerr << "PROMOTE " << candidates.cand.begin()->first << endl;
	      }
	    } else if (candidates.cand.rbegin()->first.find(candidates.cand.begin()->first)!=string::npos 
		       && ((*candidates.cand.rbegin()).first.length() - (*candidates.cand.begin()).first.length() > 2 )) {
	      wne.frenchCandidates[srcWord].cand[candidates.cand.rbegin()->first]++;
	      if (verbose) {
		cerr << srcWord << "\t *** ";
		cerr << "PROMOTE " << candidates.cand.rbegin()->first << endl;
	      }
	    }
		
	    
	  default : 
	    if (wne.frenchCandidates[srcWord].cand.size()==0 ) {
	      wne.frenchCandidates[srcWord]= candidates;
	    }
	    /*
	    if (srcWord.length() <=2 ) {
	      wne.frenchCandidates[srcWord].cand.clear();
	    }
	    */
	    //	      wne.frenchCandidates.insert(make_pair(srcWord, candidates));
	    

	    // promote true (and false) fr/en friends
	    for (map<string, int>::iterator itCand = candidates.cand.begin(); itCand!=candidates.cand.end(); itCand++) {
	      Distance lDist;
	      // compute the score without the pronoun
	      int ldScore = lDist.LD(desax(LoaderVerbsModule::desaxData, candidates.verbCand[itCand->first]),srcWord);
	      if (ldScore<=3) {
		wne.frenchCandidates[srcWord].cand[itCand->first]+=3-ldScore;	  
		if (verbose) {
		cerr << srcWord << "\t *** ";
		  cerr << "PROMOTE levenshtein " << itCand->first << " - " << wne.frenchCandidates[srcWord].cand.size() << endl;    
		}
	      }
	    }
	    break;
	  }
	}
	ss.ignore(1, ' ') ;
	ss.ignore(1, '0') ;  

      }

      char buff[2048];
      ss.getline( buff, 2048);
      wne.def=buff;
      wne.def=wne.def.substr(wne.def.rfind('|')!=string::npos?(wne.def.rfind('|')+1):wne.def.length());
      wn[synsetId]=wne;
      cnt++;

    }
  }

  dataIfs.close();
  return wn;
}

  
