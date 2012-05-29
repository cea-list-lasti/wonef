#include "WolfVerbsHandler.hpp"
#include "Tools.hpp"
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace xercesc;
using namespace std;





WolfVerbsHandler::WolfVerbsHandler(map<string, set<string> >* _wolfNet, map<string, set<string> >* _wolfNetIdIdent) :
  nbSynsets(0) {
  wolfNet = _wolfNet;


  wolfNetIdIdent = _wolfNetIdIdent;

  XMLTransService* const  theService =
    XMLPlatformUtils::fgTransService;

  XMLTransService::Codes  theCode;

  theTranscoder =
    theService->makeNewTranscoderFor(
                                         "utf-8",
                                         theCode,
                                         8192);



}

WolfVerbsHandler::~WolfVerbsHandler() {
  delete theTranscoder;
}


string WolfVerbsHandler::_transcode(const XMLCh* const chars) {
  if (*chars==10) {
    return "";
  }
  XMLSize_t length = XMLString::stringLen(chars);
  const XMLCh* it = chars;
  while (*it!=0) {
    if (it[0]>=0xa0) {
      length++;
    }
    it++;    
  }
  XMLByte buff[65536];  
  buff[0]=0;

  XMLSize_t nbTranscoded = 0;
  theTranscoder->transcodeTo (chars, length, buff, 16384, nbTranscoded, XMLTranscoder::UnRep_RepChar);
  buff[length]=0;
  char *buff2 = (char*) buff;
  XMLString::trim(buff2);
  string res = buff2;
  //  cerr << "res : " << res << endl;
  remove(res.begin(), res.end(), '*');
  remove(res.begin(), res.end(), '[');
  remove(res.begin(), res.end(), ']');
  remove(res.begin(), res.end(), '\'');
  while(res!="" && (res.find(" ")==0 || res.rfind(" ")==(res.length()-1))) {
    res=res.substr(res.find(" ")+1).substr(0,res.rfind(" "));
  } 
  return res;
}

bool WolfVerbsHandler::checkAttr(const Attributes &           attrs, string key, string value ) {
  XMLCh * _key = XMLString::transcode(key.c_str());
  bool res =_transcode(attrs.getValue(_key)).compare(value)==0 ;
  XMLString::release(&_key);
  return (res);
}

string WolfVerbsHandler::getAttrValue(const Attributes &           attrs, string value) {
  XMLCh * _key =  XMLString::transcode(value.c_str());
  if (attrs.getValue(_key)==NULL) {
    XMLString::release(&_key);
    return "";
  }
  string res = _transcode(attrs.getValue(_key));
  XMLString::release(&_key);
  return   res;
}

void WolfVerbsHandler::startElement(const XMLCh *const              uri,
                                  const XMLCh *const            localname,
                                  const XMLCh *const            qname,
                                  const Attributes &           attrs) {

  if(_transcode(qname).compare("SYNSET")==0) {
    nbSynsets++;
    if (nbSynsets%10000==0) {
      cerr << "nbSynset : " << nbSynsets << endl;
    }
  } else if(_transcode(qname).compare("SENSE")==0) {
    literal = tmpString;
  }
  
}

void WolfVerbsHandler::characters(const XMLCh *const chars, const XMLSize_t length)  {
  //  if (length<100) {    
    tmpString = _transcode(chars);
    //  }
}

void WolfVerbsHandler::endElement(const XMLCh *const uri,
                                const XMLCh *const localname,
                                const XMLCh *const qname) {
  if (_transcode(qname).compare("ID")==0) {
    id = tmpString.substr(6, 8);
  } else if (_transcode(qname).compare("POS")==0) {
    pos = tmpString;
  }else if (_transcode(qname).compare("LITERAL")==0) {
/*UNIQUE MODIF : POS = v et pas n*/
    if (pos=="v") {
      if (wolfNet->find(tolower(literal))==wolfNet->end()) {
         (*wolfNet)[tolower(literal)]=set<string>();
      }      
       cerr << "INSERT : " << tolower(literal) << " -> " << id << endl;
      (*wolfNet)[tolower(literal)].insert(id);
      if (wolfNetIdIdent->find(id)==wolfNetIdIdent->end()) {
         wolfNetIdIdent->insert(make_pair(id, set<string>()));
      }
      (*wolfNetIdIdent)[id].insert(tolower(literal));
    }
  }
}
