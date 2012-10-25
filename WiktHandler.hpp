#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/util/TransService.hpp>
#include <string>
#include <sstream>
#include <set>
#include <map>
#include <iostream>
#include <fstream>

using namespace xercesc;
using namespace std;

class WiktHandler : public DefaultHandler {

public : 

  WiktHandler(const string outputfile, const string srcLang, const string tgtLang);

  ~WiktHandler();

  void startElement(const XMLCh *const    	 uri,
		const XMLCh *const   	localname,
		const XMLCh *const   	qname,
		const Attributes &  	attrs);

  void characters(const XMLCh *const chars, const XMLSize_t length) ;

  void endElement(const XMLCh *const uri, const XMLCh *const localname, const XMLCh *const qname) ;


private :
    uint nbPage;
  ofstream ofs;
  set<string> srcStrings;
  set<string> tgtStrings;
  stringstream tmpStringstream;
  string tmpEntry;
  map<string,string> tmpAttrs;
  string def;
  const string srcLang;
  const string tgtLang;
  bool insideCDATA;
  XMLTranscoder*   theTranscoder;
  
  string _transcode(const XMLCh* const chars);
  bool checkAttr(const Attributes &  	attrs, string key, string value );
  string getAttrValue(const Attributes &  	attrs, string value);
};
