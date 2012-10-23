#include "WolfHandler.hpp"
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


WolfHandler::WolfHandler(map<string, set<string> >* _wolfNet,
                         map<string, set<string> >* _wolfNetIdIdent,
                         string _pos) :
                         nbSynsets(0), pos(_pos) {

  wolfNet = _wolfNet;
  wolfNetIdIdent = _wolfNetIdIdent;

  XMLTransService* const theService = XMLPlatformUtils::fgTransService;
  XMLTransService::Codes theCode;
  theTranscoder = theService->makeNewTranscoderFor("utf-8", theCode, 8192);

  sensemap = loadSensemap(pos);

}

WolfHandler::~WolfHandler() {
  delete theTranscoder;
}

void WolfHandler::startElement(const XMLCh *const /*uri*/,
                               const XMLCh *const /*localname*/,
                               const XMLCh *const qname,
                               const Attributes & /*attrs*/) {

  if(_transcode(qname, theTranscoder).compare("SYNSET")==0) {
    nbSynsets++;
  } else if(_transcode(qname, theTranscoder).compare("SENSE")==0) {
    literal = tmpString;
  }

}

void WolfHandler::characters(const XMLCh *const chars,
                             const XMLSize_t /*length*/)  {
    tmpString = _transcode(chars, theTranscoder);
    //cout << "TEST : " << tmpString << endl;
}

void WolfHandler::endElement(const XMLCh *const /*uri*/,
                                const XMLCh *const /*localname*/,
                                const XMLCh *const qname) {

  if (_transcode(qname, theTranscoder).compare("ID")==0) {
    id = sensemap[tmpString.substr(6, 8)];
  } else if (_transcode(qname, theTranscoder).compare("POS")==0) {
    PartOfSpeech = tmpString;
  } else if (_transcode(qname, theTranscoder).compare("LITERAL")==0) {
    if ((pos == "noun" && PartOfSpeech == "n")
      || (pos == "verb" && PartOfSpeech == "v")
      || (pos == "adj" && PartOfSpeech == "a")) {
      if (wolfNet->find(literal)==wolfNet->end()) {
         (*wolfNet)[literal]=set<string>();
      }
      // cerr << "INSERT : " << literal << " -> " << id << endl;
      (*wolfNet)[literal].insert(id);
      if (wolfNetIdIdent->find(id)==wolfNetIdIdent->end()) {
         wolfNetIdIdent->insert(make_pair(id, set<string>()));
      }
      (*wolfNetIdIdent)[id].insert(literal);
    }
  }

}
