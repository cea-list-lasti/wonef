#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

#include <iostream>
#include <fstream>


#include "WiktHandler.hpp"

using namespace xercesc;
using namespace std;

int main(int argc, char* argv[])
{
  if (argc!=3) {
    cerr << "Usage : buildBilingualDic wikt-parsed.xml output" << endl;
    return 1;
  }

  try {
    setlocale (LC_ALL,"fr_FR");

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

  char* xmlFile = argv[1];
  SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
  parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
  parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);   // optional

  cerr << argv[2] << endl;
  WiktHandler* defaultHandler = new WiktHandler(argv[2], "fr", "en");
  parser->setContentHandler(defaultHandler); 


  parser->setErrorHandler(defaultHandler);

  try {
    parser->parse(xmlFile);
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
  return 0;

}
