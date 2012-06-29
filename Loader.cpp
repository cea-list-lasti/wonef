#include "Loader.hpp"
#include "distance.hpp"
#include "Tools.hpp"
#include "../src/tools.h"
#include <iostream>
#include "boost/regex.hpp"

#include <cstdlib>

using namespace std;


set<string> LoaderModule::posList;
map<string, string> LoaderModule::desaxData;
map<string, string> LoaderModule::tgt2TgtDefs;
WORDNET::WordNetIndex LoaderModule::WNIndex;


LoaderModule::LoaderModule(string _infile, set<string>& _dicfiles, string posfile, string _pos, bool _noen) : 
  pos(_pos), noen(_noen) {
  infile = _infile;
  dicfiles = _dicfiles;
  cerr << "Loading...  " << endl;
  
  if(desaxData.size()==0) {
    initDesax(desaxData) ;
    LoaderModule::loadPOSList(posfile);
    LoaderModule::loadBilingualDic();
    LoaderModule::loadIndex();
  }
}


LoaderModule::~LoaderModule() {
}


void LoaderModule::loadIndex() {
  string indexFile = ""; 
  if (infile.find("data.") !=string::npos) {
    indexFile = infile.replace(infile.rfind("data"), 4,"index");
  } else if (infile.find("data") !=string::npos) {
    indexFile = infile.replace(infile.rfind("DAT"), 3,"IDX");
  }

  cerr << "Opening "<< indexFile << endl;
  ifstream idss(indexFile.c_str(), fstream::in);
  if (idss.fail()) {
    cerr << "Oops, " << indexFile << " doesn't exist. " << __FILE__ << ":" << __LINE__ << endl;
    exit(-1);
  }
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
      //      cerr << literal << " -> " << string(what[1].first, what[1].second) << endl;
      // update flags:
      flags |= boost::match_prev_avail;
      flags |= boost::match_not_bob;
    }
  }
  idss.close();
  cerr << "Index loaded" << endl;
}

void LoaderModule::loadPOSList(string posFile) {
  cerr << "Opening "<< posFile << endl;
  ifstream idss(posFile.c_str(), fstream::in);
  if (idss.fail()) {
    cerr << "Oops, " << posFile << " doesn't exist. " << __FILE__ << ":" << __LINE__ << endl;
    exit(-1);
  }

  string s;
  while (getline(idss, s) ) {	
    posList.insert(s);
  }
  idss.close();
}

void LoaderModule::loadBilingualDic() {

  for (set<string>::iterator itDic = dicfiles.begin(); itDic!=dicfiles.end(); itDic++) {
    cerr << "Opening "<< *itDic << endl;
    ifstream idss(itDic->c_str(), fstream::in);
    if (idss.fail()) {
      cerr << "Oops, " << *itDic << " doesn't exist. " << __FILE__ << ":" << __LINE__ << endl;
      exit(-1);
    }
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
      if (LoaderModule::posList.find(tgtWord)!=LoaderModule::posList.end()
	  || LoaderModule::posList.find(tgtWord.substr(0, tgtWord.find(' ')))!=LoaderModule::posList.end()
	  || (tgtWord[tgtWord.length()-1]=='s' && LoaderModule::posList.find(tgtWord.substr(0, tgtWord.length()-1))!=LoaderModule::posList.end())
	  ) {
	src2Tgt[tolower(s.substr(0, s.find(';')))].insert(tgtWord);
	
	if (s.find(";")+5< s.rfind(";")) {
	  tgt2TgtDefs[tgtWord]=s.substr(s.find(';')+1);
	}
      }
    }
    idss.close();
  }
}

WORDNET::TgtCandidates LoaderModule::extractCandidates(string srcWord) {
  WORDNET::TgtCandidates res;
  if (srcWord.length()<=2) {
    return res;
  }

  stringstream ss;
  srcWord = tolower(srcWord);
  for (set<string>::iterator it = src2Tgt[srcWord].begin() ; it!= src2Tgt[srcWord].end(); it++) {
    string tgtWord = *it;
    res.cand[tgtWord]=0;

    // removes the pronoun in order to compute subsequent scores
    if (pos == "verb") {
      if (tgtWord.find("se_") != string::npos) {
	res.verbCand[tgtWord] = tgtWord.substr(tgtWord.find("_")+1);
      } else if (tgtWord.find("s'") != string::npos) {
	res.verbCand[tgtWord] = tgtWord.substr(tgtWord.find("'")+1);
      } else {
	res.verbCand[tgtWord] = tgtWord;
      }
    }

  } 
  return res;
}


WORDNET::WordNet LoaderModule::load(bool verbose, int notmore) {

  WORDNET::WordNet wn;

  if (infile.find("index")!=string::npos) {  
    infile.replace(infile.rfind("index"), 5,"data");
  } else if (infile.find("IDX")!=string::npos) {  
    infile.replace(infile.rfind("IDX"), 3,"DAT");
  }

  ifstream dataIfs(infile.c_str(), fstream::in);
  if (dataIfs.fail()) {
    cerr << "Oops, " << infile << " doesn't exist. " << __FILE__ << ":" << __LINE__ << endl;
    exit(-1);
  }

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
      if (pos == "noun") {
	ss.ignore(256, 'n') ;
      } else if (pos == "verb") {
	ss.ignore(256, 'v') ;
      }
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
	  capital = true;
	}

	WORDNET::TranslationInfos translationInfos;
	translationInfos.original = srcWord;
	translationInfos.score = 1;

	if (WNIndex[srcWord].size()==0) {
	  cerr << "WARNING : "<<srcWord<<" has no id" << endl;	  
	} else if (WNIndex[srcWord].size()==1) {
	  //	  wne.frenchCandidates.insert(pair<string, WORDNET::TgtCandidates>(srcWord, candidates));
	  wne.frenchCandidates[srcWord]= candidates;
	  for (std::map<std::string, int>::iterator it = candidates.cand.begin(); it != candidates.cand.end(); it++) {
	    if (wne.frenchSynset.find(it->first)==wne.frenchSynset.end()) {
	      wne.frenchSynset[it->first]=set<WORDNET::TranslationInfos>();  
	    }
	    translationInfos.processed = "monosemous";
	    wne.frenchSynset[it->first].insert(translationInfos);
	    //wne.frenchSynset.insert(pair<string, string>(it->first, srcWord));
	    wne.newdef=tgt2TgtDefs[it->first];
	  }

	} else {
	  string longest = "";
	  switch (candidates.cand.size()) {	
	  case 0 :
	    wne.frenchCandidates[srcWord]= candidates;
	    //	    wne.frenchCandidates.insert(pair<string, WORDNET::TgtCandidates>(srcWord, candidates));
	    if (!noen || capital) {
	      if (wne.frenchSynset.find(srcWord)==wne.frenchSynset.end()) {
		wne.frenchSynset[srcWord]=set<WORDNET::TranslationInfos>();  
	      }
	      translationInfos.processed = "notranslation";
	      wne.frenchSynset[srcWord].insert(translationInfos);
	      //wne.frenchSynset.insert(pair<string, string>(srcWord, srcWord));
	    }
	    wne.newdef=tgt2TgtDefs[srcWord];
	    if (verbose) {
	      cerr << "NEWDEF : " << tgt2TgtDefs[tolower(srcWord)] << endl;
	    }
	    break;
	  case 1 :
	    wne.frenchCandidates[srcWord]=candidates;	    
	    /*
	    if (srcWord.length() <=2 ) {
	      wne.frenchCandidates[srcWord].cand.clear();
	    }
	    */
	    if (wne.frenchSynset.find(candidates.cand.begin()->first)==wne.frenchSynset.end()) {
	      wne.frenchSynset[candidates.cand.begin()->first]=set<WORDNET::TranslationInfos>();   
	    }
	    translationInfos.processed = "uniq";
	    wne.frenchSynset[candidates.cand.begin()->first].insert(translationInfos);
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
		cerr << "PROMOTE " << candidates.cand.begin()->first << endl;
	      }
	    } else if (candidates.cand.rbegin()->first.find(candidates.cand.begin()->first)!=string::npos 
		       && ((*candidates.cand.rbegin()).first.length() - (*candidates.cand.begin()).first.length() > 2 )) {	    
	      wne.frenchCandidates[srcWord].cand[candidates.cand.rbegin()->first]++;
	      if (verbose) {
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
	      int ldScore = 0;
	      if (pos == "noun") {
		ldScore = lDist.LD(desax(LoaderModule::desaxData, itCand->first),srcWord);
	      } else if (pos == "verb") {
		// compute the score without the pronoun
		ldScore = lDist.LD(desax(LoaderModule::desaxData, candidates.verbCand[itCand->first]),srcWord);
	      }
	      if (ldScore<=3) {	     
		wne.frenchCandidates[srcWord].cand[itCand->first]+=3-ldScore;	  
		if (verbose) {
		  cerr << "PROMOTE levenstein " << itCand->first << " - " << wne.frenchCandidates[srcWord].cand.size() << endl;    
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

  
