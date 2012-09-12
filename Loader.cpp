#include "Loader.hpp"
#include "distance.hpp"
#include "Tools.hpp"
#include "../src/tools.h"
#include <iostream>
#include <boost/regex.hpp>
#include <boost/foreach.hpp>

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

    cerr << "... Loaded!" << endl;
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
        WORDNET::TgtCandidates candidates = extractCandidates(srcWord);
        bool capital = false;
        if (WNIndex[srcWord].size()==0) {
          srcWord = tolower(srcWord);
          capital = true;
        }
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

        if (WNIndex[srcWord].size()==0) {
          cerr << "WARNING : "<<srcWord<<" has no id" << endl;
        } else if (WNIndex[srcWord].size()==1) {
          /* If this word only appears in one synset, let's assume the
           * translation is correct since monosemous */
          wne.frenchCandidates[srcWord] = candidates;
          for (std::map<std::string, int>::iterator it = candidates.cand.begin(); it != candidates.cand.end(); it++) {
            addInstance(wne.frenchSynset, "monosemous", it->first, srcWord, 1);
            wne.newdef=tgt2TgtDefs[it->first];
          }

        } else {
          string longest = "";
          switch (candidates.cand.size()) {

            /* There is no translation for this word, maybe use the english
             * word. TODO: is this helpful for verbs and adjectives too? */
            case 0 :
              wne.frenchCandidates[srcWord]= candidates;
              if (!noen || capital) {
                // original == translation here.
                addInstance(wne.frenchSynset, "notranslation", srcWord, srcWord, 1);
              }
              wne.newdef=tgt2TgtDefs[srcWord];
              if (verbose) {
                cerr << "NEWDEF : " << tgt2TgtDefs[tolower(srcWord)] << endl;
              }
              break;

            /* If there's only one possible translation in this synset, choose it.
             * TODO: is this really helpful? for all part-of-speech? */
            case 1 :
              wne.frenchCandidates[srcWord]=candidates;
              addInstance(wne.frenchSynset, "uniq", candidates.cand.begin()->first, srcWord, 1);

              wne.newdef=tgt2TgtDefs[candidates.cand.begin()->first];
              if (verbose) {
                cerr << "NEWDEF : " << tgt2TgtDefs[tolower(candidates.cand.begin()->first)] << endl;
              }
              break;

            /* OK so what is this? */
            case 2 :
              wne.frenchCandidates[srcWord]= candidates;
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


            /* Use Levenshtein distance to promote true and false friends. Why no "processed"? */
            default :
              if (wne.frenchCandidates[srcWord].cand.size()==0 ) {
                wne.frenchCandidates[srcWord] = candidates;
              }

              // promote true (and false) fr/en friends
              for (map<string, int>::iterator itCand = candidates.cand.begin(); itCand!=candidates.cand.end(); itCand++) {
                Distance lDist;
                int ldScore = 0;
                if (pos == "verb") {
                  // compute the score without the pronoun
                  ldScore = lDist.LD(desax(LoaderModule::desaxData, candidates.verbCand[itCand->first]),srcWord);
                } else {
                  ldScore = lDist.LD(desax(LoaderModule::desaxData, itCand->first),srcWord);
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

      /* For this synset, we read every possible verb and its candidate
       * translations. We can now look at repeated translations (eg. two
       * english verbs give the same french word) and add them to our list.
       *
       * The plan is to build something like an inverted index and to see which
       * french word lead to multiple english words */
      std::map<std::string, std::set<std::string> > englishCount;

      typedef std::pair<const std::string, int> cand_t;
      typedef std::pair<const string, WORDNET::TgtCandidates> candidate_t;

      BOOST_FOREACH(candidate_t& candidate, wne.frenchCandidates) {
        BOOST_FOREACH(cand_t& cand, candidate.second.cand) {
            englishCount[cand.first].insert(candidate.first);
        }
      }

      typedef std::pair<const std::string, std::set<std::string> > count_t;

      BOOST_FOREACH(count_t& count, englishCount) {
        if(count.second.size() > 1) {
          BOOST_FOREACH(const std::string& srcWord, count.second) {
            addInstance(wne.frenchSynset, "multiplesource", count.first, srcWord, 1);
          }
        }
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

void LoaderModule::addInstance(std::map<std::string, std::set<WORDNET::TranslationInfos> >& frenchSynset,
  const std::string& processed, const std::string& translation,
  const std::string& original, int score) {

  WORDNET::TranslationInfos translationInfos;
  translationInfos.original = original;
  translationInfos.score = score;
  translationInfos.processed = processed;

  if (frenchSynset.find(translation) == frenchSynset.end()) {
    frenchSynset[translation] = set<WORDNET::TranslationInfos>();
  }

  frenchSynset[translation].insert(translationInfos);
}
