#ifndef JAWSEVALUATORHANDLER_HPP
#define JAWSEVALUATORHANDLER_HPP


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

class JawsEvaluatorHandler : public DefaultHandler {

public : 

  JawsEvaluatorHandler(set<string>& litList, set<string>& polysemousIdsList, map<string, set<string> >& vtNet, map<string, set<string> >& vtNetIdIdent, string& datafile);

  ~JawsEvaluatorHandler();

  void startElement(const XMLCh *const    	 uri,
		const XMLCh *const   	localname,
		const XMLCh *const   	qname,
		const Attributes &  	attrs);

  void characters(const XMLCh *const chars, const XMLSize_t length) ;

  void endElement(const XMLCh *const uri, const XMLCh *const localname, const XMLCh *const qname) ;

  void endDocument(const XMLCh *const    	 uri,
		   const XMLCh *const   	localname,
		   const XMLCh *const   	qname	);

  uint nbSynsets;  
  uint nbOriginalLit ;
  uint nbInstances;
  uint cntCommonPolysemousId;

  uint cntPolysemousTermsProcessedInJaws;
  uint cntPolysemousTermsProcessedInJawsFoundInVt;
  uint cntPolysemousTermsProcessedInJawsAgreeWithVt;
  uint cntPolysemousTermsProcessedInVt;
  uint cntPolysemousTermsProcessedInVtFoundInJaws;
  uint cntPolysemousTermsProcessedInVtAgreeWithJaws;
  
  uint nbTermsInJaws;
  uint nbPolysemousTermsInJaws;
  uint nbTermsInJawsAndVt;
  uint nbPolysemousTermsInJawsAndVt;
  uint nbTermsInJawsAgreeWithVt;
  uint nbPolysemousTermsInJawsAgreeWithVt;
  
  uint cntType1;
  uint cntType2;
  uint cntType3;
  uint cntType4;

protected :

  XMLTranscoder*   theTranscoder;
  string id;
  string original;
  string originalSrc;
  string processed;
  string pos;
  string translation;
  string tmpString;
  set<string> originalsList;
  map<string, string> glosses;
  map<string, set<string> > candidates;
  map<string, set<string> > jawsNet;
  map<string, set<string> > jawsNetIdIdent;
  map<string, set<string> > vtNet;
  map<string, set<string> > vtNetIdIdent;
  set<string> litList;
  set<string> polysemousIdsList;

};



#endif
