#include "EwnLoader.hpp"
#include "Tools.hpp"
#include <boost/algorithm/string/predicate.hpp>


using namespace std;



EwnLoader::EwnLoader(map<string, set<string> >* _ewnNet, map<string, set<string> >* _ewnNetIdIdent, string& _filepath, map<string, set<string> >* _mapping) :
  filepath(_filepath), nbSynsets(0), mapping(_mapping) {
    ewnNet = _ewnNet;
    ewnNetIdIdent = _ewnNetIdIdent;
  }

EwnLoader::~EwnLoader() {
}


void EwnLoader::load () {
  cerr << "Loading from : " << filepath << endl;
  ifstream idss(filepath.c_str(), fstream::in);
  if (idss.fail()) {
    cerr << "Oops, " << filepath << " doesn't exist. " << __FILE__ << ":" << __LINE__ << endl;
    exit(-1);
  }
  string s;
  string literal = "";
  string pos = "";
  int cntSynset = 0;
  while (getline(idss, s) ) {
    // we neeed to know the PoS to safely ignore it
    if (boost::starts_with(s, "  1 PART_OF_SPEECH")) {
      // detect pos: the antepenultimate character of the string
      pos = std::string(1, s[s.size()-3]);
    }

    if (pos != "v") {
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
            //	  cerr << "TEST POS " << pos << "\n";
          }
          break;
        case 2 :
          if (string(what[2].first,what[2].second).compare("LITERAL")==0) {
            literal = string(what[4].first,what[4].second);
            //	  cerr << "TEST LITERAL " << literal << "\n";
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
            //	  cerr << "TEST TMP " << tmp << "\n";
            int j = tmp.find('-')-1;
            for (int i = 7; j > 0; i--) {
              id[i]=tmp[j];
              j--;
            }
            if (mapping->find(id) == mapping->end()) {
              cerr << "Pas de correspondance pour " << id << "\n";
            } else {
              for(std::set<std::string>::iterator itMapId = mapping->at(id).begin();
                  itMapId != mapping->at(id).end(); itMapId++){
                if(ewnNet->find(tolower(literal))==ewnNet->end()) {
                  (*ewnNet)[tolower(literal)]= set<string>();
                }
                (*ewnNet)[tolower(literal)].insert(*itMapId);

                if(ewnNetIdIdent->find(*itMapId)==ewnNet->end()) {
                  (*ewnNet)[*itMapId]= set<string>();
                }
                (*ewnNetIdIdent)[*itMapId].insert(tolower(literal));
              }
            }
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
