#include "Timer.hpp"
#include "Loader.hpp"
#include "Tools.hpp"
#include "Paths.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <cassert>

#include <cstdlib>



std::set<std::string> LoaderModule::posList;
WORDNET::WordNetIndex LoaderModule::WNIndex;


LoaderModule::LoaderModule(std::string _infile, std::string _posfile, std::string _pos) :
  pos(_pos), posfile(_posfile), infile(_infile), dictionaries(_pos) {  }

LoaderModule::~LoaderModule() {}

void LoaderModule::loadIndex() {
  std::string indexFile = "";
  if (infile.find("data.") !=std::string::npos) {
    indexFile = infile.replace(infile.rfind("data"), 4,"index");
  } else if (infile.find("data") !=std::string::npos) {
    indexFile = infile.replace(infile.rfind("DAT"), 3,"IDX");
  }

  std::ifstream idss(indexFile.c_str(), std::fstream::in);
  if (idss.fail()) {
    std::cerr << "Oops, " << indexFile << " doesn't exist. " << __FILE__ << ":" << __LINE__ << std::endl;
    exit(-1);
  }
  std::string s;
  while (getline(idss, s) ) {
    std::string literal = s.substr(0,s.find(' '));
    std::string idPattern =  " ([0-9]{8}) ";
    std::string::const_iterator start, end;
    start = s.begin();
    end = s.end();
    boost::match_results<std::string::const_iterator> what;
    boost::match_flag_type flags = boost::match_default;
    while(regex_search(start, end, what, boost::regex(idPattern), flags))  {
      boost::match_results<std::string::const_iterator> whatInside;
      start = what[1].second;
      //      end = what[0].second;
      WNIndex[literal].insert(std::string(what[1].first, what[1].second));
      // cout << literal << " -> " << std::string(what[1].first, what[1].second) << std::endl;
      // update flags:
      flags |= boost::match_prev_avail;
      flags |= boost::match_not_bob;
    }
  }
  idss.close();
}

void LoaderModule::loadPOSList(std::string posFile) {
  std::ifstream idss(posFile.c_str(), std::fstream::in);
  if (idss.fail()) {
    std::cerr << "Oops, " << posFile << " doesn't exist. " << __FILE__ << ":" << __LINE__ << std::endl;
    exit(-1);
  }

  std::string s;
  while (getline(idss, s) ) {
    posList.insert(s);
  }
  idss.close();
}


WORDNET::TgtCandidates LoaderModule::extractCandidates(std::string srcWord) {
  WORDNET::TgtCandidates res;
  if (srcWord.length()<=2) {
    return res;
  }

  std::stringstream ss;
  for (const std::string& tgtWord : dictionaries.translations[srcWord]) {
    res.cand[tgtWord]=0;

    // removes the pronoun in order to compute subsequent scores
    if (pos == "verb") {
      if (tgtWord.find("se_") != std::string::npos) {
        res.verbCand[tgtWord] = tgtWord.substr(tgtWord.find("_")+1);
      } else if (tgtWord.find("s'") != std::string::npos) {
        res.verbCand[tgtWord] = tgtWord.substr(tgtWord.find("'")+1);
      } else {
        res.verbCand[tgtWord] = tgtWord;
      }
    }

  }
  return res;
}

WORDNET::WordNet LoaderModule::load() {
  Timer t;

  LoaderModule::loadPOSList(posfile);
  LoaderModule::loadIndex();

  WORDNET::WordNet wn;

  if (infile.find("index")!=std::string::npos) {
    infile.replace(infile.rfind("index"), 5,"data");
  } else if (infile.find("IDX")!=std::string::npos) {
    infile.replace(infile.rfind("IDX"), 3,"DAT");
  }

  std::ifstream dataIfs(infile.c_str(), std::fstream::in);
  if (dataIfs.fail()) {
    std::cerr << "Oops, " << infile << " doesn't exist. " << __FILE__ << ":" << __LINE__ << std::endl;
    exit(-1);
  }

  int cnt = 0;
  std::string s = "";

  while (getline(dataIfs, s)) {
    WORDNET::WordNetEntry wne;
    wne.pos = WORDNET::POS_of_string[pos];
    wne.nbCandidates = 0;
    if (s[0]!=' ') { // not trying to read the license header
      std::string str = s;
      std::string synsetId = s.substr(0, s.find(' '));

      std::stringstream ss(s);

      if (pos == "noun") {
        ss.ignore(256, 'n') ;
      } else if (pos == "verb") {
        ss.ignore(256, 'v') ;
      } else if (pos == "adj") {
        while(ss.peek() != 'a' && ss.peek() != 's') {
          ss.ignore();
        }
        ss.ignore();
      } else if (pos == "adv") {
        ss.ignore(256, 'r');
      }
      // read nbSyns as an hexadecimal number
      int nbSyns = 0xaa;
      ss >> std::hex >> nbSyns ;

      /* Read literals */
      for (int i = 0; i < nbSyns; i++) {
        ss.ignore(1, ' ') ;
        std::string srcWord;
        ss >> srcWord;
        srcWord=srcWord.substr(0, srcWord.find(' '));

        /* Adjectives can be of the of the form "bearing(a)", and the last
         * three characters need to be removed.
         *
         * According to http://vicki.intra.cea.fr/dokuwiki/doku.php?id=lic2m:analyse_semantique#specificites_des_adjectifs
         * this only means prepositional/postpositional adjective and is not
         * expected to be useful for our translation purpose.
         */
        if (pos == "adj" && srcWord[srcWord.size()-1] == ')') {
          // we need to account for "adj(a)", "adj(p)" and "adj(ip)"
          int toremove = srcWord[srcWord.size()-3] == '(' ? 3 : 4;
          srcWord.resize(srcWord.size() - toremove);
        }

        /* Words in the index are lower-case, so this is an easy way to see if
         * the word is lower-case or not */
        std::string lowerCaseSrcWord = srcWord;
        bool capital = false;
        if (WNIndex[srcWord].size()==0) {
          lowerCaseSrcWord = tolower(srcWord);
          capital = true;
        }

        if (WNIndex[lowerCaseSrcWord].size()==0) {
          std::cerr << "ERROR: " << srcWord << " has no id" << std::endl;
          exit(-1);
        }


        wne.newdef=dictionaries.definition[srcWord];
        wne.frenchCandidates[srcWord] = extractCandidates(srcWord);
        wne.frenchCandidates[srcWord].capital = capital;
        assert(wne.frenchCandidates[srcWord].capital == capital);

        wne.nbCandidates += wne.frenchCandidates[srcWord].cand.size();

        ss.ignore(1, ' ') ;
        ss.ignore(1, '0') ;

      }

      /* Read lexical relations */
      int nbRels = 0;
      ss >> std::dec >> nbRels;

      for(int i = 0; i < nbRels; i++) {
        std::string relation, synset, pos, num;
        ss >> relation >> synset >> pos >> num;

        wne.ilr[WORDNET::ILR_of_code[relation]].insert("eng-30-" + synset + "-" + pos);
      }

      /* Read defs and usage */
      std::string buff = ss.str();
      auto defusages = readUsages(buff.substr(buff.rfind('|')+1));
      wne.def = defusages.first;
      wne.usages = defusages.second;


      wn[synsetId]=wne;
      cnt++;

    }
  }

  dataIfs.close();

  std::cout << "Loaded WordNet + dictionaries: " << t.duration() << "s" << std::endl;
  return wn;
}

void transfer_one_char(std::stringstream &defusages, std::string& usage) {
  assert(defusages.peek() != -1 && defusages.good());
  usage += defusages.get();
}

std::pair<std::string, std::vector<std::string>> LoaderModule::readUsages(std::string s) {
  std::stringstream defusages(s);
  std::string def;
  std::vector<std::string> usages;

  // read def
  while ((def.size() < 3 || def.substr(def.size()-3) != "; \"") && defusages.peek() != -1) {
    def += defusages.get();
  }
  // if we indeed have stopped because of '; "', remove that
  if (def.substr(def.size() - 3) == "; \"") {
    def = def.substr(0, def.size()-3);
  }
  boost::trim(def);

  // read usages
  while (defusages.peek() != -1) {
    std::string usage;

    // one usage between quotes (eg. "long-toed")
    while (defusages.peek() != '"' && defusages.peek() != ';' && defusages.peek() != ':' && defusages.good()) {
readquote:
      transfer_one_char(defusages, usage);
    }

    // end quote of one usage could be missing (eg. "long-toed; "five-toed")
    if (defusages.peek() == ';') {
      defusages.get();
      defusages.get();
      char peeked = defusages.peek();
      defusages.unget();
      defusages.unget();

      if (peeked != '"') { goto readquote; }
    }

    // end quote could be ':'
    if (defusages.peek() == ':') {
      defusages.get();
      if(defusages.peek() != ';') {
        goto readquote;
      }
      defusages.unget();
    }

    defusages.get();

    // quote/paren inversion: "at the time appointed (or the appointed time")
    if (defusages.peek() == ')') {
      // check if previous char was indeed a quote
      defusages.unget();
      assert(defusages.peek() == '"');
      defusages.get();

      // advance
      defusages.get();
      usage =+ ")";
    }

    // authors citations
    if (defusages.peek() == '-') {
      while(defusages.peek() != ';' && defusages.good()) {
        transfer_one_char(defusages, usage);
      }
    }

    // special case: car"women
    if (defusages.peek() == 'w') {
      std::string tmp;
      defusages >> tmp;
      assert(tmp == "women");
      break;
    }

    // special case: "I"m siding against the current candidate"
    if (defusages.peek() == 'm') {
        goto readquote;
    }

    // errors in separators
    assert(defusages.eof() || defusages.peek() == ' ' || defusages.peek() == ';' || defusages.peek() == ',' || defusages.peek() == ':' || defusages.peek() == '.');

    while (defusages.peek() != '"' && defusages.peek() != '(' && defusages.good()) {
      defusages.ignore();
    }

    // additional informations
    if (defusages.peek() == '(') {
      usage += " ";
      while(defusages.peek() != ')') {
        transfer_one_char(defusages, usage);
      }
      usage += ")";

    }

    // ignore everything up to the next quote
    while(defusages.peek() != '"' && defusages.good()) { defusages.ignore(); }
    // and the quote itself!
    defusages.ignore();

    usages.push_back(usage);


  }


  return make_pair(def, usages);
}

