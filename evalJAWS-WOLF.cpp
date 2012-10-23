#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>
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
#include "EwnLoader.hpp"
#include "GoldHandler.hpp"
#include "JawsHandler.hpp"

using namespace std;


std::map<std::string, std::set<std::string> > loadMapfile(std::string mapfile) {

  std::map<std::string, std::set<std::string> > mapping;
  std::cout << "loading mapfile from: " << mapfile << "\n";
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


void loadPolysemousLiteral(set<string>& litList, set<string>& polysemousIdsList, string filename) {

  std::cerr << "loading polysemous literals from: " << filename << std::endl;
  ifstream llss(filename.c_str(), fstream::in);
  if (llss.fail()) {
    std::cerr << "Oops, " << filename << " doesn't exist. " << __FILE__ << ":" << __LINE__ << std::endl;
    exit(-1);
  }
  string s;
  while (getline(llss, s) ) {
    litList.insert(s.substr(0, s.find(' ')));
    s=s.substr(0, s.length()-2);
    while (s.rfind(' ')==s.length()-9) {
      polysemousIdsList.insert(s.substr(s.rfind(' ')+1));
      s=s.substr(0, s.rfind(' '));
    }
  }
  llss.close();
}

SAX2XMLReader* preParser(void) {
  SAX2XMLReader* parser;
  XMLPlatformUtils::Initialize();
  parser = XMLReaderFactory::createXMLReader();
  parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
  parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);   // optional

  return parser;
}

int loadWOLF(map<string, set<string> >& wolfNet, map<string, set<string> >& wolfNetIdIdent, string filename, string spos) {

  SAX2XMLReader* parser = preParser();

  WolfHandler* wolfHandler = new WolfHandler(&wolfNet, & wolfNetIdIdent, spos);
  parser->setContentHandler(wolfHandler);
  parser->setErrorHandler(wolfHandler);
  parser->parse(filename.c_str());

  delete parser;
  delete wolfHandler;
  return 0;
}

int loadBcsBase(map<string, int>& bcsbase, map<int, int>& BCSCount, string spos) {

  SAX2XMLReader* parser = preParser();
  BcsbaseHandler* bcsbaseHandler = new BcsbaseHandler(bcsbase, BCSCount, spos);
  parser->setContentHandler(bcsbaseHandler);
  parser->setErrorHandler(bcsbaseHandler);
  parser->parse(BCSFILE.c_str());

  delete parser;
  delete bcsbaseHandler;
  return 0;
}


int loadEWN(map<string, set<string> >& ewnNet, map<string, set<string> >& ewnNetIdIdent, string filepath, map<string, set<string> >& mapping) {
  EwnLoader ewnLoader(&ewnNet, &ewnNetIdIdent, filepath, &mapping);
  ewnLoader.load();
  return 0;
}

int loadGold(map<string, set<string> >& goldNet,
             map<string, set<string> >& goldNetIdIdent,
             map<pair<string, string>, int>& goldValue,
             string filename) {

  SAX2XMLReader* parser = preParser();

  GoldHandler* goldHandler = new GoldHandler(& goldNet, & goldNetIdIdent, & goldValue);
  parser->setContentHandler(goldHandler);
  parser->setErrorHandler(goldHandler);
  parser->parse(filename.c_str());

  delete parser;
  delete goldHandler;
  return 0;
}




int parseAndEvaluatePolysemous(std::ofstream& out, map<string, int>& BCS,
                               map<int, int>& BCSCount,
                               set<string>& litList,
                               set<string>& polysemousIdsList,
                               map<string, set<string> >& vtNet,
                               map<string, set<string> >& vtNetIdIdent,
                               string pos,
                               string filename,
                               map<pair<string, string>, int>& goldValue,
                               bool gold) {

  Timer t;

  if (!boost::filesystem::exists(filename))
  {
    cerr << "Oops, " << filename << " doesn't exist. " << __FILE__ << ":" << __LINE__ << endl;
    exit(-1);
  }

  JawsHandler* jawsHandler = new JawsHandler(out, litList,
                                             polysemousIdsList,
                                             vtNet,
                                             vtNetIdIdent,
                                             goldValue,
                                             gold,
                                             pos,
                                             BCS, BCSCount);
  jawsHandler->parse_file(filename);

  delete jawsHandler;

  std::cout << "Parsed JAWS duration: " << t.duration() << "s" << std::endl;
  return 0;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    cerr << "Usage : evalJAWS-WOLF pos seqs" << endl;
    return 1;
  }

  string spos = argv[1];
  string seqs = argv[2];

  std::map<std::string, POS> POS_of_string = { {"noun", POS::Noun}, {"verb", POS::Verb}, {"adj", POS::Adj} };
  POS pos = POS_of_string[spos];

  std::map<POS, std::string> groundTruth = { {POS::Noun, WOLF}, {POS::Verb, EWN}, {POS::Adj, WOLF} };
  std::map<POS, std::string> goldFile = { { POS::Noun, GOLD_NOUN }, {POS::Verb, GOLD_VERB}, {POS::Adj, GOLD_ADJ} };

  std::string suffix = spos + "." + seqs;

  std::string jaws = "data/jaws." + suffix + ".xml";
  std::string bestJaws = "data/jaws.best." + suffix + ".xml";

  set<string> litList = set<string>();
  set<string> polysemousIdsList = set<string>();
  map<string, set<string> > vtNet = map<string, set<string> >();
  map<string, set<string> > vtNetIdIdent = map<string, set<string> >();
  map<string,int> bcsbase = map<string, int>();
  map<int,int> BCSCount;
  map<string, set<string> > mapping = map<string, set<string> >();
  map<pair<string, string>, int> goldValue = map<pair<string, string>, int>();

  Timer t;

  // BCS helps evaluation metrics
  loadBcsBase(bcsbase, BCSCount, spos);
  std::cout << "BCS duration: " << t.duration() << "s" << std::endl;

  // Loading literal list
  loadPolysemousLiteral(litList, polysemousIdsList, POLYSEMOUSINDEX + spos);

  std::string vtmode = groundTruth[pos];

  /* First evaluate with a given ground truth */
  if (vtmode == WOLF) {
    // loading WOLF
    cerr << "Loading WOLF... ";
    loadWOLF(vtNet, vtNetIdIdent, groundTruth[pos], spos);
    cerr << t.duration() << "s" << endl;
  } else if (vtmode == EWN) {
    // loading EWN
    cerr << "Loading EWN... ";
    mapping = loadMapfile(MAPVERB15_20);
    loadEWN(vtNet, vtNetIdIdent, groundTruth[pos], mapping);
    cerr << t.duration() << "s" << endl;
  }

  std::ofstream log("logs/eval." + suffix, ios_base::out | ios_base::trunc);
  parseAndEvaluatePolysemous(log, bcsbase, BCSCount, litList, polysemousIdsList,
      vtNet, vtNetIdIdent,
      spos, jaws, goldValue, false);

  std::ofstream logBest("logs/eval.best." + suffix, ios_base::out | ios_base::trunc);
  parseAndEvaluatePolysemous(logBest, bcsbase, BCSCount, litList, polysemousIdsList,
      vtNet, vtNetIdIdent,
      spos, bestJaws, goldValue, false);

  /* Then evaluate with the gold standard */
  cerr << "Loading Gold... ";
  vtNet.clear();
  vtNetIdIdent.clear();

  loadGold(vtNet, vtNetIdIdent, goldValue, goldFile[pos]);
  cerr << t.duration() << "s" << endl;

  std::ofstream logGold("logs/eval.gold." + suffix, ios_base::out | ios_base::trunc);
  parseAndEvaluatePolysemous(logGold, bcsbase, BCSCount, litList, polysemousIdsList,
      vtNet, vtNetIdIdent,
      spos, jaws, goldValue, true);
  std::ofstream logGoldBest("logs/eval.gold.best." + suffix, ios_base::out | ios_base::trunc);
  parseAndEvaluatePolysemous(logGoldBest, bcsbase, BCSCount, litList, polysemousIdsList,
      vtNet, vtNetIdIdent,
      spos, bestJaws, goldValue, true);

  return 0;
}
