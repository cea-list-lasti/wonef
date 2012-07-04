#ifndef GOLDHANDLER_HPP
#define GOLDHANDLER_HPP

#include "JawsEvaluatorHandler.hpp"

class GoldHandler : public DefaultHandler {

public : 
  GoldHandler(std::map<std::string, std::set<std::string> >* _goldNet,
	      std::map<std::string, std::set<std::string> >* _goldNetIdIdent);

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
  std::map<std::string, std::set<std::string> >* goldNet;
  std::map<std::string, std::set<std::string> >* goldNetIdIdent;

};

#endif