#include "EwnLoader.hpp"
#include "Tools.hpp"

using namespace std;



EwnLoader::EwnLoader(map<string, set<string> >* _ewnNet, map<string, set<string> >* _ewnNetIdIdent, string& _filepath) :
  filepath(_filepath), nbSynsets(0) {
  ewnNet = _ewnNet;
  ewnNetIdIdent = _ewnNetIdIdent;
}

EwnLoader::~EwnLoader() {
}


void EwnLoader::load () {
  cerr << "Loading from : " << filepath << endl;
  ifstream idss(filepath.c_str());
  string s;
  string literal = "";
  string pos = "";
  int cntSynset = 0;
  while (getline(idss, s) ) {    
    /*    if (cntSynset>100 ) {
      break;
    }
    */
    if (pos.length()>0 && pos.compare("n")!=0 && s[0]!=0) {
      continue;
    }
    string pattern =  " *([0-9]) ([A-Z_0-9@]*) (\"(.*)\")?";
    std::string::const_iterator start, end;
    start = s.begin();
    end = s.end();
    boost::match_results<std::string::const_iterator> what;
    boost::match_flag_type flags = boost::match_default;
    while(regex_search(start, end, what, boost::regex(pattern), flags))  {
      boost::match_results<std::string::const_iterator> whatInside;
      start = what[0].second;      
      stringstream ss; 
      ss << string(what[1].first,what[1].second);
      int state = -1;
      ss >> state; 
      switch (state) {
      case 0 :
	// clear all temporary data
	literal = "";
	pos = "";
	cntSynset++;
	break;
      case 1 : 
	if (string(what[2].first,what[2].second).compare("PART_OF_SPEECH")==0) {
	  pos = string(what[4].first,what[4].second);
	} 
	break;
      case 2 : 
	if (string(what[2].first,what[2].second).compare("LITERAL")==0) {	  
	  literal = string(what[4].first,what[4].second);
	} 
	break;
      case 3 : 
	break;
      case 4 : 
	break;
      case 5 : 
	if (string(what[2].first,what[2].second).compare("TEXT_KEY")==0) {	  
	  string id = "00000000";
	  string tmp(what[3].first,what[3].second);
	  int j = tmp.find('-')-1;
	  for (int i = 7; j > 0; i--) {
	    id[i]=tmp[j];
	    j--;
	  }
	  if(ewnNet->find(tolower(literal))==ewnNet->end()) {
	    (*ewnNet)[tolower(literal)]= set<string>();
	  } 
	  (*ewnNet)[tolower(literal)].insert(id);

	  if(ewnNetIdIdent->find(id)==ewnNet->end()) {
	    (*ewnNet)[id]= set<string>();
	  } 

	  (*ewnNetIdIdent)[id].insert(tolower(literal));	  
	} 
	break;
      }
      // update flags:
      flags |= boost::match_prev_avail;
      flags |= boost::match_not_bob;
    }
  }
  idss.close();


}
