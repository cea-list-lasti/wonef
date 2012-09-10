#ifndef JAWSHANDLER_HPP
#define JAWSHANDLER_HPP

#include "WordNet.hpp"
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/util/TransService.hpp>
#include <string>
#include <set>
#include <map>

class JawsHandler : public xercesc::DefaultHandler {

public :
  
  JawsHandler(std::set<std::string>& _polyLitList,
              std::set<std::string>& _polyIdsList,
              std::map<std::string, std::set<std::string> >& _vtNet,
              std::map<std::string, std::set<std::string> >& _vtNetIdIdent,
              std::map<std::pair<std::string, std::string>, int>& _goldValue,
              bool _gold, std::string _pos);

  ~JawsHandler();

  void startElement(const XMLCh *const uri,
                    const XMLCh *const localname,
                    const XMLCh *const qname,
                    const xercesc::Attributes & attrs);

  void characters(const XMLCh *const chars,
                  const XMLSize_t length);

  void endElement(const XMLCh *const uri,
                  const XMLCh *const localname,
                  const XMLCh *const qname);

  void endDocument();

  uint nbSynsets;
  uint nbJawsSynsets;
  uint nbGtSynsets;
  uint nbOriginals;
  uint nbPolyOriginals;
  uint nbTermsInJaws;
  uint nbPolyTermsInJaws;
  uint nbTermsInGt;
  uint nbPolyTermsInGt;
  // useful for recall: terms in GT, but only in a non-empty JAWS synset
  uint nbTermsInGtAndAJawsSynset;
  uint nbPolyTermsInGtAndAJawsSynset;
  uint nbTermsOk;
  uint nbPolyTermsOk;
  uint nbInJawsSynsetInGt;
  uint nbPolyInJawsSynsetInGt;

  /* per-synset average precision */
  uint nbTermsOkInSynset;
  uint nbPolyTermsOkInSynset;
  uint nbTermsInSynset;
  uint nbPolyTermsInSynset;
  float totalPercentageTermsOkInSynset;
  float totalPercentagePolyTermsOkInSynset;

protected :

  std::set<std::string> polyLitList;
  std::set<std::string> polyIdsList;
  // map<englishWord, set<frenchWords> >
  std::map<std::string, std::set<std::string> > candidates;
  // map<frenchWord, set<ids> >
  std::map<std::string, std::set<std::string> > vtNet;
  std::map<std::string, std::set<std::string> > jawsNet;
  // map<id, set<frenchWords> >
  std::map<std::string, std::set<std::string> > vtNetIdIdent;
  std::map<std::string, std::set<std::string> > jawsNetIdIdent;
  // map<pair<id, frenchWord>, value>
  std::map<std::pair<std::string, std::string>, int> goldValue;
  std::set<std::string> goldIds;
  bool gold;
  xercesc::XMLTranscoder* theTranscoder;
  WORDNET::WordNetEntry wne;
  std::string id;
  std::string original;
  std::string translation;
  std::string tmpString;
  std::string pos;

};

#endif
