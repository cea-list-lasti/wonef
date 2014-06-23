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



std::map<std::string, std::set<std::string> > loadMapfile(std::string mapfile) {

  std::map<std::string, std::set<std::string> > mapping;
  std::ifstream llss(mapfile.c_str(), std::fstream::in);
  if (llss.fail()) {
    std::cerr << "Oops, " << mapfile << " doesn't exist. " << __FILE__ << ":" << __LINE__ << std::endl;
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
void loadPolysemousLiteral(std::set<std::string>& litList, std::set<std::string>& polysemousIdsList, std::string filename) {
  std::ifstream llss(filename.c_str(), std::fstream::in);
  if (llss.fail()) {
    std::cerr << "Oops, " << filename << " doesn't exist. " << __FILE__ << ":" << __LINE__ << std::endl;
    exit(-1);
  }

  std::string s;
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

void parseAndEvaluatePolysemous(std::ofstream& out, std::map<std::string, int>& BCS,
    std::map<int, int>& BCSCount,
    std::set<std::string>& litList, std::set<std::string>& polysemousIdsList,
    std::map<std::string, std::set<std::string> >& vtNet, std::map<std::string, std::set<std::string> >& vtNetIdIdent,
    std::string pos, std::string filename,
    std::map<std::pair<std::string, std::string>, int>& goldValue, bool gold) {

  Timer t;

  if (!boost::filesystem::exists(filename))  {
    std::cerr << "Oops, " << filename << " doesn't exist. " << __FILE__ << ":" << __LINE__ << std::endl;
    exit(-1);
  }

  JawsHandler jawsHandler(out, litList, polysemousIdsList,
      vtNet, vtNetIdIdent, goldValue, gold,
      pos, BCS, BCSCount);
  jawsHandler.parse_file(filename);

  //std::cerr << "Parsed JAWS in " << t.duration() << "s" << std::endl;
}

int main(int argc, char **argv) {
  if (argc < 3) {
    std::cerr << "Usage : evalJAWS-WOLF mode pos seqs" << std::endl;
    return 1;
  }

  std::set<std::string> litList, polysemousIdsList;
  std::map<std::string, std::set<std::string>> vtNet, vtNetIdIdent;
  std::map<std::string, std::set<std::string>> wolf10Net, wolf10NetIdIdent;
  std::map<std::string, std::set<std::string>> goldNetTest, goldNetIdIdentTest;
  std::map<std::string,int> bcsbase;
  std::map<int,int> BCSCount;
  std::map<std::string, std::set<std::string>> mapping;
  std::map<std::pair<std::string, std::string>, int> goldValueTest, goldValueDev;

  Timer t, globalT;

  std::string mode = argv[1];
  std::string spos = argv[2];
  std::string seqs = argv[3];

  POS pos = WORDNET::POS_of_string[spos];

  std::map<POS, std::string> groundTruth = {
    {POS::Noun, WOLF},
    //{POS::Verb, EWN},
    {POS::Verb, WOLF},
    {POS::Adj, WOLF},
    {POS::Adv, WOLF}};

  std::map<POS, std::string> goldFile = {
    {POS::Noun, GOLD_NOUN},
    {POS::Verb, GOLD_VERB},
    {POS::Adj, GOLD_ADJ},
    {POS::Adv, GOLD_ADV}};

  std::map<POS, std::string> goldDevFile = {
    {POS::Noun, GOLD_DEV_NOUN},
    {POS::Verb, GOLD_DEV_VERB},
    {POS::Adj, GOLD_DEV_ADJ},
    {POS::Adv, GOLD_DEV_ADV}};

  std::string suffix = spos + "." + seqs;

  std::string jaws = "data/jaws." + suffix + ".xml";
  std::string bestJaws = "data/jaws.best." + suffix + ".xml";

  // BCS helps evaluation metrics
  std::cerr << "Loading BCS... ";
  BcsbaseHandler bcsbaseHandler(bcsbase, BCSCount, spos);
  bcsbaseHandler.parse_file(BCSFILE);
  std::cerr << t.duration() << "s" << std::endl;

  // Loading literal list
  loadPolysemousLiteral(litList, polysemousIdsList, POLYSEMOUSINDEX + spos);

  std::string vtmode = groundTruth[pos];

  /* First evaluate with a given ground truth */
  /*
  t.start();
  if (vtmode == WOLF) {
    std::cerr << "Loading WOLF... ";
    WolfHandler wolfHandler(vtNet, vtNetIdIdent, spos);
    wolfHandler.parse_file(groundTruth[pos]);
    std::cerr << t.duration() << "s" << std::endl;
  } else if (vtmode == EWN) {
    std::cerr << "Loading EWN... ";
    mapping = loadMapfile(MAPVERB15_20);
    EwnLoader ewnLoader(vtNet, vtNetIdIdent, groundTruth[pos], mapping);
    ewnLoader.load();
    std::cerr << t.duration() << "s" << std::endl;
  }
  */

  std::cerr << "Loading WOLF 1.0... ";
  t.start();
  WolfOneHandler wolfOneHandler(wolf10Net, wolf10NetIdIdent, spos);
  wolfOneHandler.parse_file(WOLFONE);
  std::cerr << t.duration() << "s" << std::endl;

  t.start();
  std::cerr << "Loading Gold test... ";
  GoldHandler goldHandlerTest(goldNetTest, goldNetIdIdentTest, goldValueTest);
  goldHandlerTest.parse_file(goldFile[pos]);
  std::cerr << t.duration() << "s" << std::endl;

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
  /*
  std::cout << std::endl << "-- Evaluating with Wolf 1.0... --" << std::endl;
  std::cout << std::endl << "                *** Normal ***" << std::endl;
  std::ofstream logWolfOne("logs/eval.wolfone." + suffix, ios_base::out | ios_base::trunc);
  parseAndEvaluatePolysemous(logWolfOne, bcsbase, BCSCount, litList, polysemousIdsList,
      wolf10Net, wolf10NetIdIdent, spos, jaws, goldValue, false);

  std::cout << std::endl << "                *** Filtered ***" << std::endl;
  std::ofstream logWolfOneBest("logs/eval.wolfone.best." + suffix, ios_base::out | ios_base::trunc);
  parseAndEvaluatePolysemous(logWolfOneBest, bcsbase, BCSCount, litList, polysemousIdsList,
      wolf10Net, wolf10NetIdIdent, spos, bestJaws, goldValue, false);
  */

  /* Then evaluate with the gold standard */
  if (mode == "precision") {
    std::cout << std::endl << "-- Evaluating filtered with gold... --" << std::endl;
    std::ofstream logGoldBest("logs/eval.gold.best.test." + suffix, std::ios_base::out | std::ios_base::trunc);
    parseAndEvaluatePolysemous(logGoldBest, bcsbase, BCSCount, litList, polysemousIdsList,
        goldNetTest, goldNetIdIdentTest, spos, bestJaws, goldValueTest, true);
  } else {
    std::cout << std::endl << "-- Evaluating with gold... --" << std::endl;
    std::ofstream logGold("logs/eval.gold.test." + suffix, std::ios_base::out | std::ios_base::trunc);
    parseAndEvaluatePolysemous(logGold, bcsbase, BCSCount, litList, polysemousIdsList,
        goldNetTest, goldNetIdIdentTest, spos, jaws, goldValueTest, true);
  }
  std::cout << std::endl;

  std::cout << "Overall evaluation duration: " << globalT.duration() << " s" << std::endl;

  return 0;
}
