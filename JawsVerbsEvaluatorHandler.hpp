#ifndef JAWSVERBSEVALUATORHANDLER_HPP
#define JAWSVERBSEVALUATORHANDLER_HPP


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

class JawsVerbsEvaluatorHandler : public DefaultHandler {

public : 

  JawsVerbsEvaluatorHandler(set<string>& litList, set<string>& polysemousIdsList, map<string, set<string> >& vtNet, map<string, set<string> >& vtNetIdIdent, string& datafile);

  ~JawsVerbsEvaluatorHandler();

  void startElement(const XMLCh *const    	 uri,
		const XMLCh *const   	localname,
		const XMLCh *const   	qname,
		const Attributes &  	attrs);

  void characters(const XMLCh *const chars, const XMLSize_t length) ;

  void endElement(const XMLCh *const uri, const XMLCh *const localname, const XMLCh *const qname) ;

  void endDocument(const XMLCh *const    	 uri,
		   const XMLCh *const   	localname,
		   const XMLCh *const   	qname	);

protected :
  uint nbSynsets;  
  uint nbOriginalLit ;
  uint nbInstances;
  uint cntCommonPolysemousId;

  uint cntPolysemousVerbsProcessedInJaws;
  uint cntPolysemousVerbsProcessedInJawsFoundInVt;
  uint cntPolysemousVerbsProcessedInJawsAgreeWithVt;
  uint cntPolysemousVerbsProcessedInVt;
  uint cntPolysemousVerbsProcessedInVtFoundInJaws;
  uint cntPolysemousVerbsProcessedInVtAgreeWithJaws;
  
  uint cntType1;
  uint cntType2;
  uint cntType3;
  uint cntType4;

  XMLTranscoder*   theTranscoder;
  string id;
  string original;
  string originalSrc;
  string processed;
  string pos;
  string translation;
  string tmpString;
  map<string, string> glosses;
  map<string, set<string> > candidates;
  map<string, set<string> > jawsNet;
  map<string, set<string> > jawsNetIdIdent;
  map<string, set<string> > vtNet;
  map<string, set<string> > vtNetIdIdent;
  set<string> litList;
  set<string> polysemousIdsList;
public : 
  string _transcode(const XMLCh* const chars);
  bool checkAttr(const Attributes &  	attrs, string key, string value );
  string getAttrValue(const Attributes &  	attrs, string value);

  inline uint getCntError(int type) {
    switch (type) {
    case 1 : 
      return cntType1;
    case 2 : 
      return cntType2;
    case 3 : 
      return cntType3;
    case 4 : 
      return cntType4;
    }
    return 0;
  }; 

  inline uint getCnt(int jaws, int vt) {
    switch (jaws) {

    case 0 : 
      switch (vt) {
      case 1 :
	return cntPolysemousVerbsProcessedInVt; 

      }
    case 1 : 
      switch (vt) {
      case 0 :
	return cntPolysemousVerbsProcessedInJaws ; 
      case 2 :
	return cntPolysemousVerbsProcessedInVtFoundInJaws;
      }
    case 2 : 
      switch (vt) {
      case 1 :
	return cntPolysemousVerbsProcessedInJawsFoundInVt; 
      case 2 :
	return cntPolysemousVerbsProcessedInJawsAgreeWithVt;
      }

    }
    return 0;
  };



   inline uint getNbOriginalLit() {
     return nbOriginalLit;
   };

   inline uint getCntCommonPolysemousId() {
     return cntCommonPolysemousId;
   };


};



#endif
