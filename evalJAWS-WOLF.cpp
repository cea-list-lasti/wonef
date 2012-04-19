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

using namespace std;


void loadPolysemousLiteral(set<string>& litList, set<string>& polysemousIdsList, string filename) {

  ifstream llss(filename.c_str());
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


int loadWOLFnouns(map<string, set<string> >& wolfNet, map<string, set<string> >& wolfNetIdIdent, string filename) {

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

  WolfHandler* defaultHandler = new WolfHandler(&wolfNet, & wolfNetIdIdent);
  parser->setContentHandler(defaultHandler);
  parser->setErrorHandler(defaultHandler);

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
  /* catch (...) {
    cout << "Unexpected Exception \n";
    return -1;
    }*/

  delete parser;
  delete defaultHandler;
}

int loadBcsBase15(map<string, int>& bcsbase, string filename) {
  cerr << "loading bcs from : " << filename << endl;
  cerr << "Loaded" << endl;
}

int loadBcsBaseComplem(map<string, int>& bcsbase, string filename) {
  cerr << "loading bcs from : " << filename << endl;
  cerr << "Loaded" << endl;
}



int loadBcsBase(map<string, int>& bcsbase, string filename) {
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

  BcsbaseHandler* defaultHandler = new BcsbaseHandler(&bcsbase);
  parser->setContentHandler(defaultHandler);
  parser->setErrorHandler(defaultHandler);

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
  /* catch (...) {
    cout << "Unexpected Exception \n";
    return -1;
    }*/

  delete parser;
  delete defaultHandler;
}


int loadEWNnouns(map<string, set<string> >& ewnNet, map<string, set<string> >& ewnNetIdIdent, string filepath) {
  EwnLoader ewnLoader(&ewnNet, &ewnNetIdIdent, filepath);
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

  JawsEvaluatorHandler* defaultHandler = NULL; 
  if (bcsmode==OLDSCHOOL) {
    defaultHandler = new JawsEvaluatorHandler(litList, polysemousIdsList, wolfNet, wolfNetIdIdent, datafile);
  } else {
    defaultHandler = new JawsEvaluatorBenchHandler(bcsbase, litList, polysemousIdsList, wolfNet, wolfNetIdIdent, datafile, bcsmode);
  }

  parser->setContentHandler(defaultHandler);
  parser->setErrorHandler(defaultHandler);

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
  cout << "cntPolysemousNounsProcessedInJaws : " << defaultHandler->getCnt(1, 0) << endl;
  cout << "cntPolysemousNounsProcessedInJawsFoundInWolf : " << defaultHandler->getCnt(2, 1) << endl;
  cout << "cntPolysemousNounsProcessedInJawsAgreeWithWolf : " << defaultHandler->getCnt(2, 2) << endl;
  cout << "cntCommonPolysemousId : " << defaultHandler->getCntCommonPolysemousId() << endl;
  float precision = (float)defaultHandler->getCnt(2, 2) /(float) defaultHandler->getCnt(1, 0);
  float precision2 = (float)defaultHandler->getCnt(2, 2) /(float) defaultHandler->getCnt(2, 1);
  float precision3 = (float)defaultHandler->getCnt(2, 2) /(float) defaultHandler->getCntCommonPolysemousId();

  cout << "Precision / WOLF : " << precision  << endl;
  cout << "Generous Precision / WOLF : " << precision2  << endl;
  cout << "P2/WOLF : "  << precision3 << endl;
  cout << "cntPolysemousNounsProcessedInWolf : " << defaultHandler->getCnt(0, 1) << endl;
  cout << "cntPolysemousNounsProcessedInWolfFoundInJaws : " << defaultHandler->getCnt(1, 2) << endl;
  cout << "cntPolysemousNounsProcessedInWolfAgreeWithJaws : " << defaultHandler->getCnt(2, 2) << endl;

  cout << "nbOriginalLit : " << defaultHandler->getNbOriginalLit() << endl;

  //  assert(defaultHandler->getCnt(2, 1)==defaultHandler->getCntCommonPolysemousId()) ;

  cout << "Wolf coverage : " << defaultHandler->getCnt(0, 1) << "(" << (float)defaultHandler->getCnt(0, 1)/(float)defaultHandler->getNbOriginalLit() << ")" << endl;


    // total original polysemous pairs processed in jaws, synset in wolf and pair found in wolf
  cout <<" & " 
    << ceil(((float)( defaultHandler->getCnt(1, 0))/ (float)defaultHandler->getNbOriginalLit())*1000)/10.   << "\\%"
    // total original polysemous pairs processed
    << " & "  << ceil(((float) defaultHandler->getCnt(2, 2)/ (float)defaultHandler->getCnt(1,0) )*1000)  /10. << "\\%"
       << "("<< ceil(((float) defaultHandler->getCnt(2, 2)/ (float) ( defaultHandler->getCnt(2, 1)))*1000) /10. << "\\%" << ")"
    // total original polysemous pairs processed in jaws, synset in wolf but pair not found in wolf
       << " & " << ceil(((float) ( defaultHandler->getCnt(2, 1) - defaultHandler->getCnt(2, 2))/ (float)defaultHandler->getCnt(1,0))*1000)/10. << "\\%"  
       << "("<< ceil((((float) ( defaultHandler->getCnt(2, 1) - defaultHandler->getCnt(2, 2)))/ (float) ( defaultHandler->getCnt(2, 1)))*1000) /10. << "\\%" << ")"
    // total original polysemous pairs processed in jaws, synset id not found in wolf
    << " & " << ceil(((float) ( defaultHandler->getCnt(1, 0) - defaultHandler->getCnt(2, 1))/ (float)defaultHandler->getCnt(1,0))*1000) /10. << "\\%"

       << " \\\\ " << endl;

  cout << defaultHandler->getCnt(2, 2)
    //       << "(" << (float) defaultHandler->getCnt(2, 2)/ (float)defaultHandler->getNbOriginalLit()  << ")"
       << "(" << (float) defaultHandler->getCnt(2, 2)/ (float)defaultHandler->getCnt(1,0)  << ")"
    // total original polysemous pairs processed in jaws, synset in wolf but pair not found in wolf
       << " | " << defaultHandler->getCnt(2, 1) - defaultHandler->getCnt(2, 2)
    //       << "(" << (float) ( defaultHandler->getCnt(2, 1) - defaultHandler->getCnt(2, 2))/ (float)defaultHandler->getNbOriginalLit()  << ")"
       << "(" << (float) ( defaultHandler->getCnt(2, 1) - defaultHandler->getCnt(2, 2))/ (float)defaultHandler->getCnt(1,0)  << ")"
    // total original polysemous pairs processed in jaws, synset id not found in wolf
       << " | " << defaultHandler->getCnt(1, 0) - defaultHandler->getCnt(2, 1)
    //       << "(" << (float) ( defaultHandler->getCnt(1, 0) - defaultHandler->getCnt(2, 1))/ (float)defaultHandler->getNbOriginalLit()  << ")"
       << "(" << (float) ( defaultHandler->getCnt(1, 0) - defaultHandler->getCnt(2, 1))/ (float)defaultHandler->getCnt(1,0)  << ")"
    // total original polysemous pairs processed
       << " | " << defaultHandler->getCnt(1, 0) 
       << "(" << (float)( defaultHandler->getCnt(1, 0))/ (float)defaultHandler->getNbOriginalLit()  << ")"
    // total original polysemous pairs not processed
    //       << " | " << defaultHandler->getNbOriginalLit()-defaultHandler->getCnt(1, 0) 
    //       << "(" << (float)( defaultHandler->getNbOriginalLit() - defaultHandler->getCnt(1, 0))/ (float)defaultHandler->getNbOriginalLit()  << ")"
    // total original polysemous pairs
       << " | " << defaultHandler->getNbOriginalLit() 
       << " | " << endl;


  /*  float iprecision = (float) defaultHandler->getCnt(2, 2) /  (float) defaultHandler->getCnt(0, 1);
  float iprecision2 = (float)defaultHandler->getCnt(2, 2) /  (float) defaultHandler->getCnt(1, 2);
  cout << "IPrecision / JAWS : " <<  iprecision << endl;
  cout << "Generous IPrecision / JAWS : " <<  iprecision2 << endl;

  
  cout << "averageP = " <<(precision+iprecision)/2 << endl;

  cout << "cntType1 : " << defaultHandler->getCntError(1) << endl;
  cout << "Precision1 / WOLF : " << (float)defaultHandler->getCnt(2, 2) /  (float) (defaultHandler->getCnt(1, 0) - defaultHandler->getCntError(1)) << endl;
  cout << "cntType2 : " << defaultHandler->getCntError(2) << endl;
  cout << "Precision2 / WOLF : " << (float)defaultHandler->getCnt(2, 2) /  (float) (defaultHandler->getCnt(1, 0) - defaultHandler->getCntError(2)) << endl;
  cout << "cntType3 : " << defaultHandler->getCntError(3) << endl;
  cout << "Precision3 / WOLF : " << (float)defaultHandler->getCnt(2, 2) /  (float) (defaultHandler->getCnt(0, 1) - defaultHandler->getCntError(3)) << endl;
  cout << "cntType4 : " << defaultHandler->getCntError(4) << endl;
  cout << "Precision4 / WOLF : " << (float)defaultHandler->getCnt(2, 2) /  (float) (defaultHandler->getCnt(0, 1) - defaultHandler->getCntError(4)) << endl;
  */



  delete parser;
  delete defaultHandler;
}





int main(int argc, char **argv) {
  if (argc <6) {
    cerr << "Usage : evalJAWS-WOLF  literalList wolf.xml jaws.xml vtmode bcsmode bcsfile" << endl;
    return 1;
  }

  string vtmode =argv[4];
  string datafile = DATA_NOUN;
  
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
    loadBcsBase(bcsbase, argv[6]);
  } else if (vtmode.compare("ewn")==0) {
  


    switch (bcsmode) {
    case (int)OLDSCHOOL : 
      break;
    case (int) BCS1 : 
    case (int) BCS2 : 
    case (int) BCS3 : 
      loadBcsBase15(bcsbase, argv[6]);
      break;    
    case (int) BCSALL : 
      loadBcsBaseComplem(bcsbase, argv[6]);
      break;
    }
  }



  // Loading literal list
  loadPolysemousLiteral(litList, polysemousIdsList, argv[1]);
  
  // loading WOLF
  cerr << "VTMode : " <<vtmode << endl;
  if (vtmode.compare("wolf")==0) {
    cerr << "Loading WOLF" << endl;  
    if (loadWOLFnouns(wolfNet, wolfNetIdIdent, argv[2])==1) {
      return 1;
    }
    datafile=DATA_NOUN;
  } else if (vtmode.compare("ewn")==0) {
    // loading EWN 
    cerr << "Loading EWN" << endl;  
    if (loadEWNnouns(wolfNet, wolfNetIdIdent, argv[2])==1) {
      return 1;
    }
    datafile=DATA_NOUN15;
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
