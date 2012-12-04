#include <boost/filesystem.hpp>

#include <math.h>
#include <set>
#include <map>
#include <iostream>
#include <fstream>

#include "Timer.hpp"
#include "Paths.hpp"
#include "BCSBaseHandler.hpp"
#include "WolfHandler.hpp"
#include "WolfOneHandler.hpp"
#include "EwnLoader.hpp"
#include "GoldHandler.hpp"
#include "JawsHandler.hpp"

using namespace std;


std::map<std::string, std::set<std::string> > loadMapfile(std::string mapfile) {

  std::map<std::string, std::set<std::string> > mapping;
  std::ifstream llss(mapfile.c_str(), fstream::in);
  if (llss.fail()) {
    cerr << "Oops, " << mapfile << " doesn't exist. " << __FILE__ << ":" << __LINE__ << endl;
    exit(-1);
  }
  std::string s;

  while (getline(llss, s)) {
    boost::regex patternIdSrc("([0-9]*)(.*)");
    boost::regex patternIdsTgt(" ([0-9]*) (1|0.[0-9]*)");
    std::string::const_iterator start, end;
    start = s.begin();
    end = s.end();
    boost::match_results <std::string::const_iterator> what;
    if (boost::regex_search(start, end, what, patternIdSrc)) {
      std::string idSrc = what[1].str();
      start = what[1].second;
      boost::match_results <std::string::const_iterator> whatInside;
      while (boost::regex_search(start, end, whatInside, patternIdsTgt)) {
        std::string idTgt = whatInside[1].str();
        if(mapping.find(idSrc)==mapping.end()) {
          mapping[idSrc]= std::set<std::string>();
        }
        mapping[idSrc].insert(idTgt);
        start = whatInside[1].second;
      }
    }
  }

  return(mapping);
}


/* This functions loads literals and synsets from a list of english literals
 * which belong to two or more synset. If a term is in litList, then it's a
 * polysemous term. If a synset is in polysemousIdsList, then at least one of
 * its terms is polysemous */
void loadPolysemousLiteral(set<string>& litList, set<string>& polysemousIdsList, string filename) {
  ifstream llss(filename.c_str(), fstream::in);
  if (llss.fail()) {
    std::cerr << "Oops, " << filename << " doesn't exist. " << __FILE__ << ":" << __LINE__ << std::endl;
    exit(-1);
  }

  string s;
  while (getline(llss, s) ) {
    std::string literal = s.substr(0, s.find(' '));
    litList.insert(literal);

    s=s.substr(0, s.length()-2);
    while (s.rfind(' ') == s.length()-9) {
      std::string synsetId = s.substr(s.rfind(' ')+1);
      assert(synsetId.length() == 8);
      polysemousIdsList.insert(synsetId);
      s=s.substr(0, s.rfind(' '));
    }
  }
  llss.close();
}

void parseAndEvaluatePolysemous(std::ofstream& out, map<string, int>& BCS,
    map<int, int>& BCSCount,
    set<string>& litList, set<string>& polysemousIdsList,
    map<string, set<string> >& vtNet, map<string, set<string> >& vtNetIdIdent,
    string pos, string filename,
    map<pair<string, string>, int>& goldValue, bool gold) {

  Timer t;

  if (!boost::filesystem::exists(filename))  {
    cerr << "Oops, " << filename << " doesn't exist. " << __FILE__ << ":" << __LINE__ << endl;
    exit(-1);
  }

  JawsHandler jawsHandler(out, litList, polysemousIdsList,
      vtNet, vtNetIdIdent, goldValue, gold,
      pos, BCS, BCSCount);
  jawsHandler.parse_file(filename);

  //std::cerr << "Parsed JAWS in " << t.duration() << "s" << std::endl;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    cerr << "Usage : evalJAWS-WOLF pos seqs" << endl;
    return 1;
  }

  set<string> litList, polysemousIdsList;
  map<string, set<string>> vtNet, vtNetIdIdent;
  map<string, set<string>> wolf10Net, wolf10NetIdIdent;
  map<string, set<string>> goldNet, goldNetIdIdent;
  map<string,int> bcsbase;
  map<int,int> BCSCount;
  map<string, set<string>> mapping;
  map<pair<string, string>, int> goldValue;

  Timer t, globalT;

  string spos = argv[1];
  string seqs = argv[2];

  POS pos = WORDNET::POS_of_string[spos];

  std::map<POS, std::string> groundTruth = {
    {POS::Noun, WOLF},
    //{POS::Verb, EWN},
    {POS::Verb, WOLF},
    {POS::Adj, WOLF}};

  std::map<POS, std::string> goldFile = {
    {POS::Noun, GOLD_NOUN},
    {POS::Verb, GOLD_VERB},
    {POS::Adj, GOLD_ADJ}};

  std::string suffix = spos + "." + seqs;

  std::string jaws = "data/jaws." + suffix + ".xml";
  std::string bestJaws = "data/jaws.best." + suffix + ".xml";

  // BCS helps evaluation metrics
  cerr << "Loading BCS... ";
  BcsbaseHandler bcsbaseHandler(bcsbase, BCSCount, spos);
  bcsbaseHandler.parse_file(BCSFILE);
  cerr << t.duration() << "s" << std::endl;

  // Loading literal list
  loadPolysemousLiteral(litList, polysemousIdsList, POLYSEMOUSINDEX + spos);

  std::string vtmode = groundTruth[pos];

  /* First evaluate with a given ground truth */
  /*
  t.start();
  if (vtmode == WOLF) {
    cerr << "Loading WOLF... ";
    WolfHandler wolfHandler(vtNet, vtNetIdIdent, spos);
    wolfHandler.parse_file(groundTruth[pos]);
    cerr << t.duration() << "s" << endl;
  } else if (vtmode == EWN) {
    cerr << "Loading EWN... ";
    mapping = loadMapfile(MAPVERB15_20);
    EwnLoader ewnLoader(vtNet, vtNetIdIdent, groundTruth[pos], mapping);
    ewnLoader.load();
    cerr << t.duration() << "s" << endl;
  }
  */

  cerr << "Loading WOLF 1.0... ";
  t.start();
  WolfOneHandler wolfOneHandler(wolf10Net, wolf10NetIdIdent, spos);
  wolfOneHandler.parse_file(WOLFONE);
  cerr << t.duration() << "s" << endl;

  t.start();
  cerr << "Loading Gold... ";
  GoldHandler goldHandler(goldNet, goldNetIdIdent, goldValue);
  goldHandler.parse_file(goldFile[pos]);
  cerr << t.duration() << "s" << endl;

  /* Evaluate with WOLF 0.1.4 */
  /*
  std::cout << std::endl << "-- Evaluating with Wolf 0.1.4... --" << std::endl;
  std::cout << std::endl << "                *** Normal ***" << std::endl;
  std::ofstream log("logs/eval.wolfold." + suffix, ios_base::out | ios_base::trunc);
  parseAndEvaluatePolysemous(log, bcsbase, BCSCount, litList, polysemousIdsList,
      vtNet, vtNetIdIdent, spos, jaws, goldValue, false);

  std::cout << std::endl << "                *** Filtered ***" << std::endl;
  std::ofstream logBest("logs/eval.wolfold.best." + suffix, ios_base::out | ios_base::trunc);
  parseAndEvaluatePolysemous(logBest, bcsbase, BCSCount, litList, polysemousIdsList,
      vtNet, vtNetIdIdent, spos, bestJaws, goldValue, false);
  */

  /* Then evaluate with WOLF 1.0 */
  std::cout << std::endl << "-- Evaluating with Wolf 1.0... --" << std::endl;
  std::cout << std::endl << "                *** Normal ***" << std::endl;
  std::ofstream logWolfOne("logs/eval.wolfone." + suffix, ios_base::out | ios_base::trunc);
  parseAndEvaluatePolysemous(logWolfOne, bcsbase, BCSCount, litList, polysemousIdsList,
      wolf10Net, wolf10NetIdIdent, spos, jaws, goldValue, false);

  std::cout << std::endl << "                *** Filtered ***" << std::endl;
  std::ofstream logWolfOneBest("logs/eval.wolfone.best." + suffix, ios_base::out | ios_base::trunc);
  parseAndEvaluatePolysemous(logWolfOneBest, bcsbase, BCSCount, litList, polysemousIdsList,
      wolf10Net, wolf10NetIdIdent, spos, bestJaws, goldValue, false);

  /* Then evaluate with the gold standard */
  std::cout << std::endl << "-- Evaluating with Gold... --" << std::endl;
  std::cout << std::endl << "                *** Normal ***" << std::endl;
  std::ofstream logGold("logs/eval.gold." + suffix, ios_base::out | ios_base::trunc);
  parseAndEvaluatePolysemous(logGold, bcsbase, BCSCount, litList, polysemousIdsList,
      goldNet, goldNetIdIdent, spos, jaws, goldValue, true);

  std::cout << std::endl << "                *** Filtered ***" << std::endl;
  std::ofstream logGoldBest("logs/eval.gold.best." + suffix, ios_base::out | ios_base::trunc);
  parseAndEvaluatePolysemous(logGoldBest, bcsbase, BCSCount, litList, polysemousIdsList,
      goldNet, goldNetIdIdent, spos, bestJaws, goldValue, true);
  std::cout << std::endl;

  std::cout << "Overall evaluation duration: " << globalT.duration() << " s" << std::endl;

  return 0;
}
