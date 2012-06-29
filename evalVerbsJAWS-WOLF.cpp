#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>
#include <boost/regex.hpp>

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
    cerr << "Oops, " << filename << " doesn't exist. " << __FILE__ << ":" << __LINE__ << endl;
    exit(-1);
  }
  string s;
  while (getline(llss, s) ) {
//   std::cerr << "loadPolysemousLiteral lit: '" << s.substr(0, s.find(' ')) << "'" << std::endl;
    litList.insert(s.substr(0, s.find(' ')));
    s=s.substr(0, s.length()-2);
//     std::cerr << "    ids: ";
    while (s.rfind(' ')==s.length()-9) {
//       std::cerr << s.substr(s.rfind(' ')+1) << " ";
      polysemousIdsList.insert(s.substr(s.rfind(' ')+1));
      s=s.substr(0, s.rfind(' '));
    }
//    std::cerr << std::endl;
  }
  llss.close();
}


int loadWOLFverbs(map<string, set<string> >& wolfNet, map<string, set<string> >& wolfNetIdIdent, string filename, string pos) {

  try {

    XMLPlatformUtils::Initialize();
  }
  catch (const XMLException& toCatch) {
    char* message = XMLString::transcode(toCatch.getMessage());
    cout << "Error during initialization! :\n";
    cout << "Exception message 0 is: \n"
         << message << "\n";
    XMLString::release(&message);
    return 1;
  }

  SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
  parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
  parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);   // optional

  WolfHandler* wolfHandler = new WolfHandler(&wolfNet, & wolfNetIdIdent, pos);
  parser->setContentHandler(wolfHandler);
  parser->setErrorHandler(wolfHandler);

  try {
    parser->parse(filename.c_str());
  }
  catch (const XMLException& toCatch) {
    char* message = XMLString::transcode(toCatch.getMessage());
    cout << "WOLF Exception message 1 is: \n"
         << message << "\n";
    XMLString::release(&message);
    return -1;
  }
  catch (const SAXParseException& toCatch) {
    char* message = XMLString::transcode(toCatch.getMessage());
    cout << "WOLF Exception message 2 is: \n"
         << message << "\n";
    XMLString::release(&message);
    return -1;
  }
  catch (...) {
    cout << "WOLF Unexpected Exception \n";
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
  try {

    XMLPlatformUtils::Initialize();
  }
  catch (const XMLException& toCatch) {   
    char* message = XMLString::transcode(toCatch.getMessage());
    cout << "Error during initialization! :\n";
    cout << "Exception message 0 is: \n"
         << message << "\n";
    XMLString::release(&message);
    return 1;
  }

  SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
  parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
  parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);   // optional

  BcsbaseHandler* bcsbaseHandler = new BcsbaseHandler(&bcsbase, pos);
  parser->setContentHandler(bcsbaseHandler);
  parser->setErrorHandler(bcsbaseHandler);

  try {
    parser->parse(filename.c_str());
  }
  catch (const XMLException& toCatch) {
    char* message = XMLString::transcode(toCatch.getMessage());
    cout << "Exception message 1 is: \n"
         << message << "\n";
    XMLString::release(&message);
    return -1;
  }
  catch (const SAXParseException& toCatch) {
    char* message = XMLString::transcode(toCatch.getMessage());
    cout << "BCS Exception message 2 is: \n"
         << message << "\n";
    XMLString::release(&message);
    return -1;
  }
  /* catch (...) {
    cout << "Unexpected Exception \n";
    return -1;
    }*/

  delete parser;
  delete bcsbaseHandler;
  return 0;
}


int loadEWNverbs(map<string, set<string> >& ewnNet, map<string, set<string> >& ewnNetIdIdent, string filepath, map<string, set<string> >& mapping) {
  EwnLoader ewnLoader(&ewnNet, &ewnNetIdIdent, filepath, &mapping);
  ewnLoader.load();
  return 0;
}






int parseAndEvaluatePolysemous(map<string, int>& bcsbase,
			       set<string>& litList,
			       set<string>& polysemousIdsList,
			       map<string, set<string> >& wolfNet,
			       map<string, set<string> >& wolfNetIdIdent, 
			       string filename,
			       string& datafile,
			       BCSMode bcsmode) {


  try {

    XMLPlatformUtils::Initialize();
  }
  catch (const XMLException& toCatch) {
    char* message = XMLString::transcode(toCatch.getMessage());
    cout << "Error during initialization! :\n";
    cout << "Exception message 0 is: \n"
         << message << "\n";
    XMLString::release(&message);
    return 1;
  }

  SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
  parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
  parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);   // optional

  JawsEvaluatorHandler* jawsEvaluatorHandler = NULL; 
  if (bcsmode==OLDSCHOOL) {
    jawsEvaluatorHandler = new JawsEvaluatorHandler(litList, polysemousIdsList, wolfNet, wolfNetIdIdent, datafile);
  } else {
    jawsEvaluatorHandler = new JawsEvaluatorBenchHandler(bcsbase, litList, polysemousIdsList, wolfNet, wolfNetIdIdent, datafile, bcsmode);
  }

  parser->setContentHandler(jawsEvaluatorHandler);
  parser->setErrorHandler(jawsEvaluatorHandler);

  try {
    parser->parse(filename.c_str());
  }
  catch (const XMLException& toCatch) {
    char* message = XMLString::transcode(toCatch.getMessage());
    cout << "Exception message 1 is: \n"
         << message << "\n";
    XMLString::release(&message);
    return -1;
  }
  catch (const SAXParseException& toCatch) {
    char* message = XMLString::transcode(toCatch.getMessage());
    cout << "Exception message 2 is: \n"
         << message << "\n";
    XMLString::release(&message);
    return -1;
  }

  cout << "------------------------------------" << endl;
  cout << "cntPolysemousVerbsProcessedInJaws : " 
       << jawsEvaluatorHandler->cntPolysemousTermsProcessedInJaws << endl;
  cout << "cntPolysemousVerbsProcessedInJawsFoundInEWN : " 
       << jawsEvaluatorHandler->cntPolysemousTermsProcessedInJawsFoundInVt << endl;
  cout << "cntPolysemousVerbsProcessedInJawsAgreeWithEWN : " 
       << jawsEvaluatorHandler->cntPolysemousTermsProcessedInJawsAgreeWithVt << endl;
  cout << "cntCommonPolysemousId : " 
       << jawsEvaluatorHandler->cntCommonPolysemousId << endl;
  float precision = (float)jawsEvaluatorHandler->cntPolysemousTermsProcessedInJawsAgreeWithVt
		   /(float) jawsEvaluatorHandler->cntPolysemousTermsProcessedInJaws;
  float precision2 = (float)jawsEvaluatorHandler->cntPolysemousTermsProcessedInJawsAgreeWithVt
		    /(float) jawsEvaluatorHandler->cntPolysemousTermsProcessedInJawsFoundInVt;
  float precision3 = (float)jawsEvaluatorHandler->cntPolysemousTermsProcessedInJawsAgreeWithVt
		    /(float) jawsEvaluatorHandler->cntCommonPolysemousId;

  cout << "Precision / EWN : " << precision  << endl;
  cout << "Generous Precision / EWN : " << precision2  << endl;
  cout << "P2/WOLF : "  << precision3 << endl;
  cout << "cntPolysemousVerbsProcessedInEWN : " 
       << jawsEvaluatorHandler->cntPolysemousTermsProcessedInVt << endl;
  cout << "cntPolysemousVerbsProcessedInEWNFoundInJaws : " 
       << jawsEvaluatorHandler->cntPolysemousTermsProcessedInVtFoundInJaws << endl;
  cout << "cntPolysemousVerbsProcessedInEWNAgreeWithJaws : " 
       << jawsEvaluatorHandler->cntPolysemousTermsProcessedInVtAgreeWithJaws << endl;

  cout << "nbOriginalLit : " << jawsEvaluatorHandler->nbOriginalLit << endl;

  //  assert(jawsEvaluatorHandler->getCnt(2, 1)==jawsEvaluatorHandler->getCntCommonPolysemousId()) ;

  cout << "EWN coverage : " << jawsEvaluatorHandler->cntPolysemousTermsProcessedInVt 
       << "(" << (float)jawsEvaluatorHandler->cntPolysemousTermsProcessedInVt
		/(float)jawsEvaluatorHandler->nbOriginalLit << ")" << endl;


    // total original polysemous pairs processed in jaws, synset in wolf and pair found in wolf
  cout <<" & " 
    << ceil(((float)( jawsEvaluatorHandler->cntPolysemousTermsProcessedInJaws)/ (float)jawsEvaluatorHandler->nbOriginalLit)*1000)/10.   << "\\%"
    // total original polysemous pairs processed
    << " & "  << ceil(((float) jawsEvaluatorHandler->cntPolysemousTermsProcessedInJawsAgreeWithVt/ (float)jawsEvaluatorHandler->cntPolysemousTermsProcessedInJaws )*1000)  /10. << "\\%"
       << "("<< ceil(((float) jawsEvaluatorHandler->cntPolysemousTermsProcessedInJawsAgreeWithVt/ (float) ( jawsEvaluatorHandler->cntPolysemousTermsProcessedInJawsFoundInVt))*1000) /10. << "\\%" << ")"
    // total original polysemous pairs processed in jaws, synset in wolf but pair not found in wolf
       << " & " << ceil(((float) ( jawsEvaluatorHandler->cntPolysemousTermsProcessedInJawsFoundInVt - jawsEvaluatorHandler->cntPolysemousTermsProcessedInJawsAgreeWithVt)/ (float)jawsEvaluatorHandler->cntPolysemousTermsProcessedInJaws)*1000)/10. << "\\%"
       << "("<< ceil((((float) ( jawsEvaluatorHandler->cntPolysemousTermsProcessedInJawsFoundInVt - jawsEvaluatorHandler->cntPolysemousTermsProcessedInJawsAgreeWithVt))/ (float) ( jawsEvaluatorHandler->cntPolysemousTermsProcessedInJawsFoundInVt))*1000) /10. << "\\%" << ")"
    // total original polysemous pairs processed in jaws, synset id not found in wolf
    << " & " << ceil(((float) ( jawsEvaluatorHandler->cntPolysemousTermsProcessedInJaws - jawsEvaluatorHandler->cntPolysemousTermsProcessedInJawsFoundInVt)/ (float)jawsEvaluatorHandler->cntPolysemousTermsProcessedInJaws)*1000) /10. << "\\%"

       << " \\\\ " << endl;

  cout << jawsEvaluatorHandler->cntPolysemousTermsProcessedInJawsAgreeWithVt
    //       << "(" << (float) jawsEvaluatorHandler->getCnt(2, 2)/ (float)jawsEvaluatorHandler->getNbOriginalLit()  << ")"
       << "(" << (float) jawsEvaluatorHandler->cntPolysemousTermsProcessedInJawsAgreeWithVt/ (float)jawsEvaluatorHandler->cntPolysemousTermsProcessedInJaws  << ")"
    // total original polysemous pairs processed in jaws, synset in wolf but pair not found in wolf
       << " | " << jawsEvaluatorHandler->cntPolysemousTermsProcessedInJawsFoundInVt - jawsEvaluatorHandler->cntPolysemousTermsProcessedInJawsAgreeWithVt
    //       << "(" << (float) ( jawsEvaluatorHandler->getCnt(2, 1) - jawsEvaluatorHandler->getCnt(2, 2))/ (float)jawsEvaluatorHandler->getNbOriginalLit()  << ")"

       << "(" << (float) ( jawsEvaluatorHandler->cntPolysemousTermsProcessedInJawsFoundInVt - jawsEvaluatorHandler->cntPolysemousTermsProcessedInJawsAgreeWithVt)/ (float)jawsEvaluatorHandler->cntPolysemousTermsProcessedInJaws  << ")"
    // total original polysemous pairs processed in jaws, synset id not found in wolf
       << " | " << jawsEvaluatorHandler->cntPolysemousTermsProcessedInJaws - jawsEvaluatorHandler->cntPolysemousTermsProcessedInJawsFoundInVt
    //       << "(" << (float) ( jawsEvaluatorHandler->getCnt(1, 0) - jawsEvaluatorHandler->getCnt(2, 1))/ (float)jawsEvaluatorHandler->getNbOriginalLit()  << ")"
       << "(" << (float) ( jawsEvaluatorHandler->cntPolysemousTermsProcessedInJaws - jawsEvaluatorHandler->cntPolysemousTermsProcessedInJawsFoundInVt)/ (float)jawsEvaluatorHandler->cntPolysemousTermsProcessedInJaws  << ")"
    // total original polysemous pairs processed
       << " | " << jawsEvaluatorHandler->cntPolysemousTermsProcessedInJaws 
       << "(" << (float)( jawsEvaluatorHandler->cntPolysemousTermsProcessedInJaws)/ (float)jawsEvaluatorHandler->nbOriginalLit  << ")"
    // total original polysemous pairs not processed
    //       << " | " << jawsEvaluatorHandler->getNbOriginalLit()-jawsEvaluatorHandler->getCnt(1, 0)
    //       << "(" << (float)( jawsEvaluatorHandler->getNbOriginalLit() - jawsEvaluatorHandler->getCnt(1, 0))/ (float)jawsEvaluatorHandler->getNbOriginalLit()  << ")"
    // total original polysemous pairs
       << " | " << jawsEvaluatorHandler->nbOriginalLit
       << " | " << endl;


  /*  float iprecision = (float) jawsEvaluatorHandler->getCnt(2, 2) /  (float) jawsEvaluatorHandler->getCnt(0, 1);
  float iprecision2 = (float)jawsEvaluatorHandler->getCnt(2, 2) /  (float) jawsEvaluatorHandler->getCnt(1, 2);
  cout << "IPrecision / JAWS : " <<  iprecision << endl;
  cout << "Generous IPrecision / JAWS : " <<  iprecision2 << endl;

  
  cout << "averageP = " <<(precision+iprecision)/2 << endl;

  cout << "cntType1 : " << jawsEvaluatorHandler->getCntError(1) << endl;
  cout << "Precision1 / WOLF : " << (float)jawsEvaluatorHandler->getCnt(2, 2) /  (float) (jawsEvaluatorHandler->getCnt(1, 0) - jawsEvaluatorHandler->getCntError(1)) << endl;
  cout << "cntType2 : " << jawsEvaluatorHandler->getCntError(2) << endl;
  cout << "Precision2 / WOLF : " << (float)jawsEvaluatorHandler->getCnt(2, 2) /  (float) (jawsEvaluatorHandler->getCnt(1, 0) - jawsEvaluatorHandler->getCntError(2)) << endl;
  cout << "cntType3 : " << jawsEvaluatorHandler->getCntError(3) << endl;
  cout << "Precision3 / WOLF : " << (float)jawsEvaluatorHandler->getCnt(2, 2) /  (float) (jawsEvaluatorHandler->getCnt(0, 1) - jawsEvaluatorHandler->getCntError(3)) << endl;
  cout << "cntType4 : " << jawsEvaluatorHandler->getCntError(4) << endl;
  cout << "Precision4 / WOLF : " << (float)jawsEvaluatorHandler->getCnt(2, 2) /  (float) (jawsEvaluatorHandler->getCnt(0, 1) - jawsEvaluatorHandler->getCntError(4)) << endl;
  */

  cout << "------------------------------------" << endl;
  cout << "\t\t*** ALL VERBS ***" << endl;
  cout << "nbVerbsInJaws :\t\t\t"
       << jawsEvaluatorHandler->nbTermsInJaws << endl;
  cout << "nbVerbsInJawsAndVt :\t\t"
       << jawsEvaluatorHandler->nbTermsInJawsAndVt << endl;
  cout << "nbVerbsInJawsAgreeWithVt :\t"
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

  cout << "\t\t*** POLYSEMOUS ***" << endl;
  cout << "nbPolysemousVerbsInJaws :\t\t"
       << jawsEvaluatorHandler->nbPolysemousTermsInJaws << endl;
  cout << "nbPolysemousVerbsInJawsAndVt :\t\t"
       << jawsEvaluatorHandler->nbPolysemousTermsInJawsAndVt << endl;
  cout << "nbPolysemousVerbsInJawsAgreeWithVt :\t"
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
  if (argc <6) {
    cerr << "Usage : evalJAWS-WOLF  literalList wolf.xml jaws.xml vtmode bcsmode bcsfile" << endl;
    return 1;
  }

  string vtmode =argv[4];
  string datafile = DATA_VERB;
  std::string mapfile = MAPVERB15_20;
  string pos = "verb";
  
  int bcsmode; 
  stringstream ss; 
  ss << argv[5];
  ss >> bcsmode; 
  
    


  set<string> litList = set<string>();
  set<string> polysemousIdsList = set<string>();
  map<string, set<string> > wolfNet = map<string, set<string> >();  
  map<string, set<string> > wolfNetIdIdent = map<string, set<string> >();
  map<string,int> bcsbase = map<string, int>();


  // loading BCS Base
  if (vtmode.compare("wolf")==0) {
    loadBcsBase(bcsbase, argv[6], pos);
  } else if (vtmode.compare("ewn")==0) {
  


    switch (bcsmode) {
    case (int)OLDSCHOOL : // 4
      break;
    case (int) BCS1 : // 1
    case (int) BCS2 : // 2
    case (int) BCS3 : // 3
      loadBcsBase15(bcsbase, argv[6]);
      break;    
    case (int) BCSALL : // 0
      loadBcsBaseComplem(bcsbase, argv[6]);
      break;
    }
  }

  // Loading Mapping File
  std::map<std::string, std::set<std::string> > mapping = loadMapfile(mapfile);

  // Loading literal list
  loadPolysemousLiteral(litList, polysemousIdsList, argv[1]);

  // loading VT
  cerr << "VTMode : " <<vtmode << endl;
  if (vtmode.compare("wolf")==0) {
    cerr << "Loading WOLF" << endl;  
    if (loadWOLFverbs(wolfNet, wolfNetIdIdent, argv[2], pos)==1) {
      return 1;
    }
    datafile=DATA_VERB;
  } else if (vtmode.compare("ewn")==0) {
    // loading EWN 
    cerr << "Loading EWN" << endl;  
    if (loadEWNverbs(wolfNet, wolfNetIdIdent, argv[2], mapping)==1) {
      return 1;
    }
    datafile=DATA_VERB15;
  }


  // parsing JAWS and evaluating
  cerr << "Parsing JAWS" << endl;
  if (parseAndEvaluatePolysemous(bcsbase,
				 litList,
				 polysemousIdsList,
				 wolfNet,
				 wolfNetIdIdent,
				 argv[3], 
				 datafile, 
				 (BCSMode) bcsmode) == 1 ) {
    return 1;
  }

  return 0;
}
