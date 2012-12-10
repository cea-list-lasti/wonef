#include "Loader.hpp"
#include "Tools.hpp"
#include "Paths.hpp"
#include <iostream>
#include <fstream>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <cassert>

#include <cstdlib>

using namespace std;


set<string> LoaderModule::posList;
map<string, string> LoaderModule::tgt2TgtDefs;
WORDNET::WordNetIndex LoaderModule::WNIndex;


LoaderModule::LoaderModule(string _infile, string posfile, string _pos) :
  pos(_pos), infile(_infile) {
    cerr << "Loading...  " << endl;
    LoaderModule::loadPOSList(posfile);
    LoaderModule::loadBilingualDic();
    LoaderModule::loadIndex();
    cerr << "... Loaded!" << endl;
  }

LoaderModule::~LoaderModule() {}

void LoaderModule::loadIndex() {
  string indexFile = "";
  if (infile.find("data.") !=string::npos) {
    indexFile = infile.replace(infile.rfind("data"), 4,"index");
  } else if (infile.find("data") !=string::npos) {
    indexFile = infile.replace(infile.rfind("DAT"), 3,"IDX");
  }

  cerr << "Opening index "<< indexFile << endl;
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
      // cout << literal << " -> " << string(what[1].first, what[1].second) << endl;
      // update flags:
      flags |= boost::match_prev_avail;
      flags |= boost::match_not_bob;
    }
  }
  idss.close();
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

bool LoaderModule::validPos(string candidatePos) {
  if (pos == "noun") { return candidatePos == "S" || candidatePos == "NP"; }
  else if (pos == "verb") { return candidatePos == "V"; }
  else if (pos == "adj") { return candidatePos == "J"; }
  else if (pos == "adv") { return candidatePos == "D"; }
  else { exit(-1); }
}

void LoaderModule::loadBilingualDic() {

  std::set<std::string> dicfiles{EURADIC, WIKTIONARY};

  for (std::string dicfile : dicfiles) {
    cerr << "Opening "<< dicfile << endl;
    ifstream idss(dicfile, fstream::in);
    if (idss.fail()) {
      cerr << "Oops, " << dicfile << " doesn't exist. " << __FILE__ << ":" << __LINE__ << endl;
      exit(-1);
    }
    string s;

    while (getline(idss, s) ) {
      s=boost::regex_replace(s, boost::regex(" "), "_");

      vector<string> strs;
      boost::split(strs, s, boost::is_any_of(";"));

      string tgtWord = strs[0];
      string tgtPos = strs[1];
      string srcWord = strs[3];
      string srcPos = strs[4];

      // insert if Wiktionary or if ELDA and valid part-of-speech
      if (dicfile == WIKTIONARY || validPos(srcPos) || validPos(tgtPos)) {
        src2Tgt[srcWord].insert(tgtWord);
      }

      // store definition for Wiktionary
      if (strs.size() >= 6) {
        tgt2TgtDefs[tgtWord]=strs[5];
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

WORDNET::WordNet LoaderModule::load() {

  WORDNET::WordNet wn;

  if (infile.find("index")!=string::npos) {
    infile.replace(infile.rfind("index"), 5,"data");
  } else if (infile.find("IDX")!=string::npos) {
    infile.replace(infile.rfind("IDX"), 3,"DAT");
  }

  cerr << "Opening data "<< infile << endl;
  ifstream dataIfs(infile.c_str(), fstream::in);
  if (dataIfs.fail()) {
    cerr << "Oops, " << infile << " doesn't exist. " << __FILE__ << ":" << __LINE__ << endl;
    exit(-1);
  }

  int cnt = 0;
  string s = "";
  cerr << infile << endl;

  while (getline(dataIfs, s)) {
    WORDNET::WordNetEntry wne;
    wne.pos = WORDNET::POS_of_string[pos];
    wne.nbCandidates = 0;
    if (s[0]!=' ') { // not trying to read the license header
      string str = s;
      string synsetId = s.substr(0, s.find(' '));

      stringstream ss(s);

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
      ss >> hex >> nbSyns ;

      /* Read literals */
      for (int i = 0; i < nbSyns; i++) {
        ss.ignore(1, ' ') ;
        string srcWord;
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
        string lowerCaseSrcWord = srcWord;
        bool capital = false;
        if (WNIndex[srcWord].size()==0) {
          lowerCaseSrcWord = tolower(srcWord);
          capital = true;
        }

        if (WNIndex[lowerCaseSrcWord].size()==0) {
          cerr << "ERROR: " << srcWord << " has no id" << endl;
          exit(-1);
        }


        wne.newdef=tgt2TgtDefs[srcWord];
        wne.frenchCandidates[srcWord] = extractCandidates(srcWord);
        wne.frenchCandidates[srcWord].capital = capital;
        assert(wne.frenchCandidates[srcWord].capital == capital);

        wne.nbCandidates += wne.frenchCandidates[srcWord].cand.size();

        ss.ignore(1, ' ') ;
        ss.ignore(1, '0') ;

      }

      /* Read lexical relations */
      int nbRels = 0;
      ss >> dec >> nbRels;

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
  return wn;
}

void transfer_one(std::stringstream &defusages, std::string& usage) {
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
  // work that should be done by the regex: if we indeed have stopped because of '; "', remove it
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
      transfer_one(defusages, usage);
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
        transfer_one(defusages, usage);
      }
    }

    // special case: car"women
    if (defusages.peek() == 'w') {
      std::string tmp;
      defusages >> tmp;
      assert(tmp == "women");
      break;
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
        transfer_one(defusages, usage);
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

