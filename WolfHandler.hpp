#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/util/TransService.hpp>
#include <string>
#include <set>
#include <map>
#include <iostream>
#include <fstream>

using namespace xercesc;
using namespace std;

class WolfHandler : public DefaultHandler {

public : 

  WolfHandler(map<string, set<string> >* wolfNet,
	      map<string, set<string> >* wolfNetIdIdent,
	      string _pos);

  ~WolfHandler();

  void startElement(const XMLCh *const uri,
		    const XMLCh *const localname,
		    const XMLCh *const qname,
		    const Attributes & attrs);

  void characters(const XMLCh *const chars,
		  const XMLSize_t length);

  void endElement(const XMLCh *const uri,
		  const XMLCh *const localname,
		  const XMLCh *const qname);


private :
  uint nbSynsets;  
  XMLTranscoder*   theTranscoder;
  string id;
  string PartOfSpeech;
  string literal;
  string tmpString;
  string pos;
  map<string, set<string> >* wolfNet;
  map<string, set<string> >* wolfNetIdIdent;
  std::map<std::string, std::string> sensemap;
};
