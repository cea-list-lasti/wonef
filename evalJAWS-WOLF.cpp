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
#include "JawsEvaluatorHandler.hpp"
#include "JawsEvaluatorHandlerBench.hpp"
#include "GoldHandler.hpp"

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
  catch (...) {
    cout << what << " Unexpected Exception \n";
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

int loadBcsBase15(map<string, int>& /*bcsbase*/, string filename) {
  cerr << "loading bcs 15 from : " << filename << endl;
  cerr << "NOTHING TO DO" << endl;
  cerr << "Loaded" << endl;
  return 0;
}

int loadBcsBaseComplem(map<string, int>& /*bcsbase*/, string filename) {
  cerr << "loading bcs complem from : " << filename << endl;
  cerr << "NOTHING TO DO" << endl;
  cerr << "Loaded" << endl;
  return 0;
}



int loadBcsBase(map<string, int>& bcsbase, string filename, string pos) {
  cerr << "loading bcs from : " << filename << endl;
  
  SAX2XMLReader* parser = NULL;
  if (preParser("BCS", parser) == 1) {
    return 1;
  }

  BcsbaseHandler* bcsbaseHandler = new BcsbaseHandler(&bcsbase, pos);
  parser->setContentHandler(bcsbaseHandler);
  parser->setErrorHandler(bcsbaseHandler);

  if (postParser ("BCS", filename, parser) == -1) {
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
             string filename) {

  SAX2XMLReader* parser = NULL;
  if (preParser("Gold", parser) == 1) {
    return 1;
  }

  GoldHandler* goldHandler = new GoldHandler(& goldNet, & goldNetIdIdent);
  parser->setContentHandler(goldHandler);
  parser->setErrorHandler(goldHandler);

  if (postParser("Gold", filename, parser) == -1) {
    return -1;
  }

  delete parser;
  delete goldHandler;
  return 0;
}




int parseAndEvaluatePolysemous(map<string, int>& bcsbase,
                               set<string>& litList,
                               set<string>& polysemousIdsList,
                               map<string, set<string> >& wolfNet,
                               map<string, set<string> >& wolfNetIdIdent,
                               string pos,
                               string filename,
                               string& datafile,
                               BCSMode bcsmode) {

  SAX2XMLReader* parser = NULL;
  if (preParser("Jaws", parser) == 1) {
    return 1;
  }

  JawsEvaluatorHandler* jawsEvaluatorHandler = NULL; 
  if (bcsmode==OLDSCHOOL) {
    jawsEvaluatorHandler = new JawsEvaluatorHandler(litList, polysemousIdsList, wolfNet, wolfNetIdIdent, datafile);
  } else {
    jawsEvaluatorHandler = new JawsEvaluatorBenchHandler(bcsbase, litList, polysemousIdsList, wolfNet, wolfNetIdIdent, datafile, bcsmode);
  }

  parser->setContentHandler(jawsEvaluatorHandler);
  parser->setErrorHandler(jawsEvaluatorHandler);

  if (postParser("Jaws", filename, parser) == -1) {
    return -1;
  }

  string terms;
  if (pos == "noun") {
    terms = "Nouns";
  } else if (pos == "verb") {
    terms = "Verbs";
  }

  cout << "------------------------------------" << endl;
  cout << "\t\t*** All " + terms + " ***" << endl;
  cout << "nb" + terms + "InJaws :\t\t\t"
       << jawsEvaluatorHandler->nbTermsInJaws << endl;
  cout << "nb" + terms + "InJawsAndVt :\t\t"
       << jawsEvaluatorHandler->nbTermsInJawsAndVt << endl;
  cout << "nb" + terms + "InJawsAgreeWithVt :\t"
       << jawsEvaluatorHandler->nbTermsInJawsAgreeWithVt << endl;

  float allPrecision = (float)jawsEvaluatorHandler->nbTermsInJawsAgreeWithVt
                      / (float)jawsEvaluatorHandler->nbTermsInJaws;
  float allPseudoPrec = (float)jawsEvaluatorHandler->nbTermsInJawsAgreeWithVt
                       / (float)jawsEvaluatorHandler->nbTermsInJawsAndVt;
  float polyPrecision = (float)jawsEvaluatorHandler->nbPolysemousTermsInJawsAgreeWithVt
                      / (float)jawsEvaluatorHandler->nbPolysemousTermsInJaws;
  float polyPseudoPrec = (float)jawsEvaluatorHandler->nbPolysemousTermsInJawsAgreeWithVt
                       / (float)jawsEvaluatorHandler->nbPolysemousTermsInJawsAndVt;

  cout << "Precision : " << allPrecision << "\t\t"
       << ceil(allPrecision*1000)/10. << "%" << endl;
  cout << "Pseudo precision : " << allPseudoPrec<< "\t" 
       << ceil(allPseudoPrec*1000)/10. << "%"  << endl;

  cout << "\t\t*** Polysemous ***" << endl;
  cout << "nbPolysemous" + terms + "InJaws :\t\t"
       << jawsEvaluatorHandler->nbPolysemousTermsInJaws << endl;
  cout << "nbPolysemous" + terms + "InJawsAndVt :\t\t"
       << jawsEvaluatorHandler->nbPolysemousTermsInJawsAndVt << endl;
  cout << "nbPolysemous" + terms + "InJawsAgreeWithVt :\t"
       << jawsEvaluatorHandler->nbPolysemousTermsInJawsAgreeWithVt << endl;

  cout << "Precision : " << polyPrecision << "\t\t\t" 
       << ceil(polyPrecision*1000)/10. << "%" << endl;
  cout << "Pseudo precision : " << polyPseudoPrec << "\t\t" 
       << ceil(polyPseudoPrec*1000)/10. << "%" << endl;



  delete parser;
  delete jawsEvaluatorHandler;
  return 0;
}





int main(int argc, char **argv) {
  if (argc < 7) {
    cerr << "Usage : evalJAWS-WOLF pos literalList vt.xml jaws.xml vtmode bcsmode bcsfile" << endl;
    return 1;
  }

  string vtmode = argv[5];
  string pos = argv[1];
  string datafile;
  string mapfile;
  if (pos == "noun") {
    datafile = DATA_NOUN;
  } else if (pos == "verb") {
    datafile = DATA_VERB;
    mapfile = MAPVERB15_20;
  }

  int bcsmode; 
  stringstream ss; 
  ss << argv[6];
  ss >> bcsmode; 




  set<string> litList = set<string>();
  set<string> polysemousIdsList = set<string>();
  map<string, set<string> > vtNet = map<string, set<string> >();  
  map<string, set<string> > vtNetIdIdent = map<string, set<string> >();
  map<string,int> bcsbase = map<string, int>();
  map<string, set<string> > mapping = map<string, set<string> >();


  // loading BCS Base
  if (vtmode.compare("wolf")==0) {
    loadBcsBase(bcsbase, argv[7], pos);
  } else if (vtmode.compare("ewn")==0) {



    switch (bcsmode) {
    case (int)OLDSCHOOL : // 4
      break;
    case (int) BCS1 : // 1
    case (int) BCS2 : // 2
    case (int) BCS3 : // 3
      loadBcsBase15(bcsbase, argv[7]);
      break;    
    case (int) BCSALL : // 0
      loadBcsBaseComplem(bcsbase, argv[7]);
      break;
    }
  }

  // Loading literal list
  loadPolysemousLiteral(litList, polysemousIdsList, argv[2]);

  cerr << "VTMode : " << vtmode << endl;
  // loading WOLF
  if (vtmode.compare("wolf")==0) {
    cerr << "Loading WOLF" << endl;  
    if (loadWOLF(vtNet, vtNetIdIdent, argv[3], pos)==1) {
      return 1;
    }
  } else if (vtmode.compare("ewn")==0) {
    // Loading Mapping File
    mapping = loadMapfile(mapfile);
    // loading EWN 
    cerr << "Loading EWN" << endl;  
    if (loadEWN(vtNet, vtNetIdIdent, argv[3], mapping)==1) {
      return 1;
    }
    if (pos == "verb") {
      datafile = DATA_VERB15;
    }
  } else if (vtmode.compare("gold")==0) {
    //loading gold
    cerr << "Loading Gold" << endl;
    if (loadGold(vtNet, vtNetIdIdent, argv[3])==1) {
      return 1;
    }
  }


  // parsing JAWS and evaluating
  cerr << "Parsing JAWS" << endl;
  if (parseAndEvaluatePolysemous(bcsbase,
                                 litList,
                                 polysemousIdsList,
                                 vtNet,
                                 vtNetIdIdent,
                                 pos,
                                 argv[4], 
                                 datafile, 
                                 (BCSMode) bcsmode) == 1 ) {
    return 1;
  }
  
  return 0;
}
