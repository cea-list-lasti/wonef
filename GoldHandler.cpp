#include "GoldHandler.hpp"
#include "Tools.hpp"
#include <boost/lexical_cast.hpp>

GoldHandler::GoldHandler(std::map<std::string, std::set<std::string> >* _goldNet,
                         std::map<std::string, std::set<std::string> >* _goldNetIdIdent,
                         std::map<std::pair<std::string, std::string>, int>* _goldValue) {

  goldNet = _goldNet;
  goldNetIdIdent = _goldNetIdIdent;
  goldValue = _goldValue;
  nbSynsets = 0;

  xercesc::XMLTransService* const theService = xercesc::XMLPlatformUtils::fgTransService;
  xercesc::XMLTransService::Codes theCode;
  theTranscoder = theService->makeNewTranscoderFor("utf-8", theCode, 8192);

}

GoldHandler::~GoldHandler(){
  delete theTranscoder;
}

void GoldHandler::characters(const XMLCh *const chars,
                             const XMLSize_t /*length*/)  {

  tmpString = _transcode(chars, theTranscoder);

}

void GoldHandler::startElement(const XMLCh *const /*uri*/,
                               const XMLCh *const /*localname*/,
                               const XMLCh*const qname,
                               const Attributes & attrs) {

  if(_transcode(qname, theTranscoder).compare("SYNSET") == 0) {
    nbSynsets++;
    id = getAttrValue(attrs, "id", theTranscoder);
  } else if(_transcode(qname, theTranscoder).compare("CANDIDATE") == 0) {
    string val = getAttrValue(attrs, "valide", theTranscoder);
    valide = boost::lexical_cast<int>(val);
  }

}

void GoldHandler::endElement(const XMLCh *const /*uri*/,
                             const XMLCh *const /*localname*/,
                             const XMLCh*const qname) {

  if (_transcode(qname, theTranscoder).compare("CANDIDATE") == 0) {
    if (valide == 1) {
      if (goldNet->find(tolower(tmpString)) == goldNet->end()) {
        (*goldNet)[tolower(tmpString)] = set<string>();
      }
      (*goldNet)[tolower(tmpString)].insert(id);
      if (goldNetIdIdent->find(id) == goldNetIdIdent->end()) {
        goldNetIdIdent->insert(make_pair(id, set<string>()));
      }
      (*goldNetIdIdent)[id].insert(tolower(tmpString));
    }

    pair<string, string> pairSynsetWord = pair<string, string>(id, tolower(tmpString));
    (*goldValue)[pairSynsetWord] = valide;
  }

}
