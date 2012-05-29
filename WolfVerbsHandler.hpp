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

class WolfVerbsHandler : public DefaultHandler {

public : 

  WolfVerbsHandler(map<string, set<string> >* wolfNet, map<string, set<string> >* wolfNetIdIdent);

  ~WolfVerbsHandler();

  void startElement(const XMLCh *const    	 uri,
		const XMLCh *const   	localname,
		const XMLCh *const   	qname,
		const Attributes &  	attrs);

  void characters(const XMLCh *const chars, const XMLSize_t length) ;

  void endElement(const XMLCh *const uri, const XMLCh *const localname, const XMLCh *const qname) ;


private :
  uint nbSynsets;  
  XMLTranscoder*   theTranscoder;
  string id;
  string pos;
  string literal;
  string tmpString;
  map<string, set<string> >* wolfNet;
  map<string, set<string> >* wolfNetIdIdent;
  
  string _transcode(const XMLCh* const chars);
  bool checkAttr(const Attributes &  	attrs, string key, string value );
  string getAttrValue(const Attributes &  	attrs, string value);
};
