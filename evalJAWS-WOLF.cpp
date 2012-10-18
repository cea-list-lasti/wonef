#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

#include <math.h>
#include <set>
#include <map>
#include <iostream>
#include <fstream>

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

int preParser(string what, SAX2XMLReader*& parser) {
  try {
    XMLPlatformUtils::Initialize();
  }
  catch (const XMLException& toCatch) {
    char* message = XMLString::transcode(toCatch.getMessage());
    cout << what << " Error during initialization! :\n";
    cout << what << " Exception message 0 is: \n"
         << message << "\n";
    XMLString::release(&message);
    return 1;
  }

  parser = XMLReaderFactory::createXMLReader();
  parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
  parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);   // optional
  return 0;
}

int postParser(string what, string filename, SAX2XMLReader*& parser) {
  try {
    parser->parse(filename.c_str());
  }
  catch (const XMLException& toCatch) {
    char* message = XMLString::transcode(toCatch.getMessage());
    cout << what << " Exception message 1 is: \n"
         << message << "\n";
    XMLString::release(&message);
    return -1;
  }
  catch (const SAXParseException& toCatch) {
    char* message = XMLString::transcode(toCatch.getMessage());
    cout << what << " Exception message 2 is: \n"
         << message << "\n";
    XMLString::release(&message);
    return -1;
  }
  return 0;
}

int loadWOLF(map<string, set<string> >& wolfNet, map<string, set<string> >& wolfNetIdIdent, string filename, string pos) {

  SAX2XMLReader* parser = NULL;
  if (preParser("Wolf", parser) == 1) {
    return 1;
  }

  WolfHandler* wolfHandler = new WolfHandler(&wolfNet, & wolfNetIdIdent, pos);
  parser->setContentHandler(wolfHandler);
  parser->setErrorHandler(wolfHandler);

  if (postParser ("Wolf", filename, parser) == -1) {
    return -1;
  }

  delete parser;
  delete wolfHandler;
  return 0;
}

int loadBcsBase(map<string, int>& bcsbase, map<int, int>& BCSCount, string pos) {

  SAX2XMLReader* parser = NULL;
  if (preParser("BCS", parser) == 1) {
    return 1;
  }

  BcsbaseHandler* bcsbaseHandler = new BcsbaseHandler(bcsbase, BCSCount, pos);
  parser->setContentHandler(bcsbaseHandler);
  parser->setErrorHandler(bcsbaseHandler);

  if (postParser ("BCS", BCSFILE, parser) == -1) {
    return -1;
  }

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

  SAX2XMLReader* parser = NULL;
  if (preParser("Gold", parser) == 1) {
    return 1;
  }

  GoldHandler* goldHandler = new GoldHandler(& goldNet, & goldNetIdIdent, & goldValue);
  parser->setContentHandler(goldHandler);
  parser->setErrorHandler(goldHandler);

  if (postParser("Gold", filename, parser) == -1) {
    return -1;
  }

  delete parser;
  delete goldHandler;
  return 0;
}




int parseAndEvaluatePolysemous(map<string, int>& BCS,
                               map<int, int>& BCSCount,
                               set<string>& litList,
                               set<string>& polysemousIdsList,
                               map<string, set<string> >& vtNet,
                               map<string, set<string> >& vtNetIdIdent,
                               string pos,
                               string filename,
                               map<pair<string, string>, int>& goldValue,
                               bool gold) {

  SAX2XMLReader* parser = NULL;
  if (preParser("Jaws", parser) == 1) {
    return 1;
  }

  JawsHandler* jawsHandler = new JawsHandler(litList,
                                             polysemousIdsList,
                                             vtNet,
                                             vtNetIdIdent,
                                             goldValue,
                                             gold,
                                             pos,
                                             BCS, BCSCount);
  parser->setContentHandler(jawsHandler);
  parser->setErrorHandler(jawsHandler);

  if (postParser("Jaws", filename, parser) == -1) {
    return -1;
  }


  delete parser;
  delete jawsHandler;
  return 0;
}





int main(int argc, char **argv) {
  if (argc < 5) {
    cerr << "Usage : evalJAWS-WOLF pos literalList vt.xml jaws.xml vtmode" << endl;
    return 1;
  }

  bool gold = false;
  string vtmode = argv[5];
  string pos = argv[1];
  string mapfile;
  if (pos == "verb") {
    mapfile = MAPVERB15_20;
  }


  set<string> litList = set<string>();
  set<string> polysemousIdsList = set<string>();
  map<string, set<string> > vtNet = map<string, set<string> >();
  map<string, set<string> > vtNetIdIdent = map<string, set<string> >();
  map<string,int> bcsbase = map<string, int>();
  map<int,int> BCSCount;
  map<string, set<string> > mapping = map<string, set<string> >();
  map<pair<string, string>, int> goldValue = map<pair<string, string>, int>();


  // No use of BCS for now
  loadBcsBase(bcsbase, BCSCount, pos);

  // Loading literal list
  loadPolysemousLiteral(litList, polysemousIdsList, argv[2]);

  cerr << "VTMode : " << vtmode << endl;
  // loading WOLF
  if (vtmode.compare("WOLF")==0) {
    cerr << "Loading WOLF" << endl;
    if (loadWOLF(vtNet, vtNetIdIdent, argv[3], pos)==1) {
      return 1;
    }
  } else if (vtmode.compare("EWN")==0) {
    // Loading Mapping File
    if (pos == "verb") {
      mapping = loadMapfile(mapfile);
    }
    // loading EWN
    cerr << "Loading EWN" << endl;
    if (loadEWN(vtNet, vtNetIdIdent, argv[3], mapping)==1) {
      return 1;
    }
  } else if (vtmode.compare("GOLD")==0) {
    //loading gold
    cerr << "Loading Gold" << endl;
    if (loadGold(vtNet, vtNetIdIdent, goldValue, argv[3])==1) {
      return 1;
    }
    gold = true;
  }


  // parsing JAWS and evaluating
  cerr << "Parsing JAWS" << endl;
  if (parseAndEvaluatePolysemous(bcsbase, BCSCount,
                                 litList,
                                 polysemousIdsList,
                                 vtNet,
                                 vtNetIdIdent,
                                 pos,
                                 argv[4],
                                 goldValue,
                                 gold) == 1 ) {
    return 1;
  }

  return 0;
}
