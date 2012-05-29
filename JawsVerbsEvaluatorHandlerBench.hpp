#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/util/TransService.hpp>
#include <string>
#include <set>
#include <map>
#include <iostream>
#include <fstream>
#include "JawsVerbsEvaluatorHandler.hpp"


using namespace xercesc;
using namespace std;


typedef enum {BCSALL, BCS1, BCS2, BCS3, OLDSCHOOL } BCSMode;

class JawsVerbsEvaluatorBenchHandler : public JawsVerbsEvaluatorHandler {

public : 
  JawsVerbsEvaluatorBenchHandler(map<string, int>& bcsbase, 
			    set<string>& litList, 
			    set<string>& polysemousIdsList,
			    map<string, set<string> >& vtNet, 
			    map<string, set<string> >& vtNetIdIdent, 
			    string& datafile,
			    BCSMode mode);

  ~JawsVerbsEvaluatorBenchHandler();


protected : 
  map<string, int> bcsbase;
  BCSMode bcsmode;
 
  void endElement(const XMLCh *const uri, const XMLCh *const localname, const XMLCh *const qname) ;


};
