#include "EwnLoader.hpp"
#include "Tools.hpp"
#include <boost/algorithm/string/predicate.hpp>



EwnLoader::EwnLoader(std::map<std::string, std::set<std::string>>& _ewnNet,
    std::map<std::string, std::set<std::string> >& _ewnNetIdIdent,
    std::string& _filepath, std::map<std::string, std::set<std::string> >& _mapping) :
  ewnNet(_ewnNet), ewnNetIdIdent(_ewnNetIdIdent),
  filepath(_filepath), nbSynsets(0), mapping(_mapping) {

  }

void EwnLoader::load () {
  //std::cerr << "Loading from : " << filepath << std::endl;
  std::ifstream idss(filepath.c_str(), std::fstream::in);
  if (idss.fail()) {
    std::cerr << "Oops, " << filepath << " doesn't exist. " << __FILE__ << ":" << __LINE__ << std::endl;
    exit(-1);
  }
  std::string s;
  std::string literal = "";
  std::string pos = "";
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
    std::string pattern =  " *([0-9]) ([A-Z_0-9@]*) (\"(.*)\")?";
    std::string::const_iterator start, end;
    start = s.begin();
    end = s.end();
    boost::match_results<std::string::const_iterator> what;
    boost::match_flag_type flags = boost::match_default;
    while(regex_search(start, end, what, boost::regex(pattern), flags))  {
      boost::match_results<std::string::const_iterator> whatInside;
      start = what[0].second;
      std::stringstream ss;
      ss << std::string(what[1].first,what[1].second);
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

          if (std::string(what[2].first,what[2].second).compare("PART_OF_SPEECH")==0) {
            pos = std::string(what[4].first,what[4].second);
            //	  std::cerr << "TEST POS " << pos << "\n";
          }
          break;
        case 2 :
          if (std::string(what[2].first,what[2].second).compare("LITERAL")==0) {
            literal = std::string(what[4].first,what[4].second);
            // we want "se gonfler" to appear as "se_gonfler" to be able to compare with JAWS
            if (boost::starts_with(literal, "se ")) {
              literal[2] = '_';
            }
            //	  std::cerr << "TEST LITERAL " << literal << "\n";
          }
          break;
        case 3 :
          break;
        case 4 :
          break;
        case 5 :
          if (std::string(what[2].first,what[2].second).compare("TEXT_KEY")==0) {
            std::string id = "00000000";
            std::string tmp(what[3].first,what[3].second);
            //	  std::cerr << "TEST TMP " << tmp << "\n";
            int j = tmp.find('-')-1;
            for (int i = 7; j > 0; i--) {
              id[i]=tmp[j];
              j--;
            }
            if (mapping.find(id) == mapping.end()) {
              //std::cerr << "Pas de correspondance pour " << id << "\n";
            } else {
              for (std::string mapped_id : mapping.at(id)) {
                ewnNet[literal].insert(mapped_id);
                ewnNetIdIdent[mapped_id].insert(tolower(literal));
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
