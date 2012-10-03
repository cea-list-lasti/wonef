#include "Loader.hpp"
#include "Tools.hpp"
#include <iostream>
#include <fstream>
#include <boost/regex.hpp>
#include <cassert>

#include <cstdlib>

using namespace std;


set<string> LoaderModule::posList;
map<string, string> LoaderModule::tgt2TgtDefs;
WORDNET::WordNetIndex LoaderModule::WNIndex;


LoaderModule::LoaderModule(string _infile, set<string>& _dicfiles, string posfile, string _pos) :
  pos(_pos), dicfiles(_dicfiles), infile(_infile) {
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
      src2Tgt[s.substr(0, s.find(';'))].insert(tgtWord);

      if (s.find(";")+5< s.rfind(";")) {
        tgt2TgtDefs[tgtWord]=s.substr(s.find(';')+1);
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


WORDNET::WordNet LoaderModule::load(bool /*verbose*/, int notmore) {

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
  cerr << "cnt : " << cnt <<"/"<< notmore << endl;

  while (getline(dataIfs, s)  && (cnt < notmore || notmore==-1)) {
    WORDNET::WordNetEntry wne;
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
      }
      // read nbSyns as an hexadecimal number
      int nbSyns = 0xaa;
      ss >> hex >> nbSyns ;

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

