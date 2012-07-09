#ifndef GOLDHANDLER_HPP
#define GOLDHANDLER_HPP

#include "JawsEvaluatorHandler.hpp"

class GoldHandler : public DefaultHandler {

public : 
  GoldHandler(std::map<std::string, std::set<std::string> >* _goldNet,
              std::map<std::string, std::set<std::string> >* _goldNetIdIdent,
              std::map<std::pair<std::string, std::string>, int>* _goldValue);

  ~GoldHandler();

  void startElement(const XMLCh *const uri,
                    const XMLCh *const localname,
                    const XMLCh *const qname,
                    const Attributes & attrs);

  void characters(const XMLCh *const chars,
                  const XMLSize_t length) ;

  void endElement(const XMLCh *const uri,
                  const XMLCh *const localname,
                  const XMLCh *const qname);

protected :
  uint nbSynsets;
  int valide;
  string id;
  string tmpString;
  xercesc::XMLTranscoder* theTranscoder;
  // map<frenchWord, set<ids> >
  std::map<std::string, std::set<std::string> >* goldNet;
  // map<id, set<frenchWords> >
  std::map<std::string, std::set<std::string> >* goldNetIdIdent;
  // map<pair<id, frenchWord>, value>
  std::map<std::pair<std::string, std::string>, int>* goldValue;

};

#endif