#include "JawsVerbsEvaluatorHandler.hpp"
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "Paths.hpp"
#include "Tools.hpp"
#include "boost/regex.hpp"

using namespace xercesc;
using namespace std;



JawsVerbsEvaluatorHandler::JawsVerbsEvaluatorHandler( set<string>& litList, set<string>& _polysemousIdsList, map<string, set<string> >& _vtNet, map<string, set<string> >& _vtNetIdIdent, string& datafile) :
  nbSynsets(0), nbOriginalLit(0), nbInstances(0), cntCommonPolysemousId(0),
  cntPolysemousVerbsProcessedInJaws(0),
  cntPolysemousVerbsProcessedInJawsFoundInVt(0),
  cntPolysemousVerbsProcessedInJawsAgreeWithVt(0),
  cntPolysemousVerbsProcessedInVt(0),
  cntPolysemousVerbsProcessedInVtFoundInJaws(0),
  cntPolysemousVerbsProcessedInVtAgreeWithJaws(0),
  cntType1(0),
  cntType2(0),
  cntType3(0),
  cntType4(0),
  vtNet(_vtNet), vtNetIdIdent(_vtNetIdIdent),
  litList(litList), polysemousIdsList(_polysemousIdsList) {


  candidates = map<string, set<string > >();
  XMLTransService* const  theService =
    XMLPlatformUtils::fgTransService;

  XMLTransService::Codes  theCode;

  theTranscoder =
    theService->makeNewTranscoderFor(
                                         "utf-8",
                                         theCode,
                                         8192);

  ifstream idss(datafile.c_str(), fstream::in);
  if (idss.fail()) {
    cerr << "Oops, " << datafile << " doesn't exist. " << __FILE__ << ":" << __LINE__ << endl;
    exit(-1);
  }
  string s;
  while (getline(idss, s) ) {         
    glosses[s.substr(0, s.find(' '))]=s.substr(s.find(' '));
  }
  idss.close();

}

JawsVerbsEvaluatorHandler::~JawsVerbsEvaluatorHandler() {
  delete theTranscoder;
}


string JawsVerbsEvaluatorHandler::_transcode(const XMLCh* const chars) {
  //  cerr << "start transcode " << endl;
  XMLSize_t length = XMLString::stringLen(chars);
  const XMLCh* it = chars;
  while (*it!=0) {
    if (it[0]>=0xa0) {
      length++;
    }
    it++;    
  }
  XMLByte buff[65536];
  XMLSize_t nbTranscoded = 0;
  theTranscoder->transcodeTo (chars, length, buff, 16384, nbTranscoded, XMLTranscoder::UnRep_RepChar);
  buff[length]=0;
  char *buff2 = (char*) buff;
  XMLString::trim(buff2);
  string res = buff2;
  remove(res.begin(), res.end(), '*');
  remove(res.begin(), res.end(), '[');
  remove(res.begin(), res.end(), ']');
  //  remove(res.begin(), res.end(), '\'');
  while(res!="" && (res.find(" ")==0 || res.rfind(" ")==(res.length()-1))) {
    res=res.substr(res.find(" ")+1).substr(0,res.rfind(" "));
  } 
  //  cerr << res << endl;
  return res;
}

bool JawsVerbsEvaluatorHandler::checkAttr(const Attributes &           attrs, string key, string value ) {
  XMLCh * _key = XMLString::transcode(key.c_str());
  bool res =_transcode(attrs.getValue(_key)).compare(value)==0 ;
  XMLString::release(&_key);
  return (res);
}

string JawsVerbsEvaluatorHandler::getAttrValue(const Attributes &           attrs, string value) {
  XMLCh * _key =  XMLString::transcode(value.c_str());
  if (attrs.getValue(_key)==NULL) {
    XMLString::release(&_key);
    return "";
  }
  string res = _transcode(attrs.getValue(_key));
  XMLString::release(&_key);
  return   res;
}

void JawsVerbsEvaluatorHandler::startElement(const XMLCh *const              /*uri*/,
                                             const XMLCh *const            /*localname*/,
                                             const XMLCh *const            qname,
                                             const Attributes &           attrs) {
  //  cerr << "START : " << _transcode(qname) << endl;
  if(_transcode(qname).compare("SYNSET")==0) {
    nbSynsets++;
    if (nbSynsets%10000==0) {
      cerr << "nbSynset : " << nbSynsets << endl;
    }
    id=getAttrValue(attrs, "id");
  } else if(_transcode(qname).compare("CANDIDATES")==0) {
    originalSrc=getAttrValue(attrs, "original");
  } else if (_transcode(qname).compare("INSTANCES")==0){
    translation = getAttrValue(attrs, "translation");
  } else if(_transcode(qname).compare("INSTANCE")==0) {  
    original=getAttrValue(attrs, "original");
    processed=getAttrValue(attrs, "processed"); 
  }
  
}

void JawsVerbsEvaluatorHandler::characters(const XMLCh *const chars, const XMLSize_t /*length*/)  {
  tmpString = _transcode(chars);
}

void JawsVerbsEvaluatorHandler::endElement(const XMLCh *const /*uri*/,
                                          const XMLCh *const /*localname*/,
                                          const XMLCh *const qname) {
  if (_transcode(qname).compare("INSTANCE")==0) {
      if (litList.find(original)!=litList.end()) {
         cntPolysemousVerbsProcessedInJaws++;
         nbInstances++;
         while (translation.find('_')!=string::npos) {
           translation=translation.replace(translation.find('_'), 1, " ");
         }
         translation=tolower(translation);
         cerr << "Testing : " << translation << " : " << id << endl;
         for (set<string>::iterator ittest = vtNet[translation].begin(); ittest !=vtNet[translation].end() ; ittest++) {
         cerr <<         "Inside : " <<*ittest<< endl;
         }
      jawsNet[translation].insert(id);
      jawsNetIdIdent[id].insert(translation);

	if (vtNetIdIdent[id].size() > 0) {
	  cntPolysemousVerbsProcessedInJawsFoundInVt++;
	  if (vtNet[translation].find(id)!=vtNet[translation].end()) {
	    cntPolysemousVerbsProcessedInJawsAgreeWithVt++;
	  } else {
	    cntType2++;
	    cout <<":Error Type 2 : "<< translation<<"("<<id << " : " << processed << " : " << original <<": ";
	    for (set<string>::iterator itCand = candidates[original].begin(); itCand!= candidates[original].end(); itCand++) {
	      cout << *itCand << ", ";
	    }
	    cout << ") exists in vt not in " << id << " : " ;
	    for (set<string>::iterator itIdent = jawsNetIdIdent[id].begin(); itIdent!= jawsNetIdIdent[id].end(); itIdent++) {
	      cout << *itIdent << ", " ;
	    }
	    cout << ". "<< endl;
	    cout <<  id << " : " << glosses[id] << endl;
	    cout << " , where they prefer : " ;
	    for (set<string>::iterator itIdent = vtNetIdIdent[id].begin(); itIdent!= vtNetIdIdent[id].end(); itIdent++) {
	      cout << *itIdent << ", " ;
	      if (translation.compare(*itIdent)==0) {
		cout << "MATCH point" << endl;
	      }
	    }

	    cout << ". "<< endl;
	    cout << ", but in " ;
	    for (set<string>::iterator itId = vtNet[translation].begin(); itId!= vtNet[translation].end(); itId++) {
	      cout << *itId << " : " ;
	      for (set<string>::iterator itIdent = vtNetIdIdent[*itId].begin(); itIdent!= vtNetIdIdent[*itId].end(); itIdent++) {
		cout << *itIdent << ", " ;
	      }
	      cout << ". "<< endl;
	      cout <<  *itId << " : " << glosses[*itId] << endl;
	    }
	    cout << ". " << endl;
	    cout << "------"<< endl;
	  }
	} else {
	  cntType1++;
	  cout << id <<":Error Type 1 : '"<< id <<"'("<< translation<< " : " << processed<< " : " << original <<":";
	  for (set<string>::iterator itCand = candidates[original].begin(); itCand!= candidates[original].end(); itCand++) {
	    cout << *itCand << ", ";
	  }
	  cout << ") does not exist in vt." << endl;
	  cout << "In Jaws :  "<< endl;
	  for (set<string>::iterator itId = jawsNet[translation].begin(); itId != jawsNet[translation].end(); itId++) {
	    cout << *itId <<" : " << glosses[*itId] << endl;
	  }
	  assert(vtNetIdIdent[id].size()==0);
	  cout << ". "<< endl;
	  cout << "------"<< endl;
	}
      }

  } else if (_transcode(qname).compare("CANDIDATES")==0) {
    if (litList.find(originalSrc)!=litList.end()) {
      cerr << "o:" << originalSrc << endl;
      nbOriginalLit++;
    }

  } else if (_transcode(qname).compare("CANDIDATE")==0) {
    candidates[originalSrc].insert(tmpString);    
  } else if (_transcode(qname).compare("SYNSET")==0) {
    if (polysemousIdsList.find(id)!=polysemousIdsList.end() ) {
      for (set<string>::iterator it = vtNetIdIdent[id].begin() ; it != vtNetIdIdent[id].end(); it++ ) {
         cntPolysemousVerbsProcessedInVt++;
	 string tgtWord = *it;
	 tgtWord = boost::regex_replace(tgtWord, boost::regex(" "), "_");
	 cerr << "TEST : " << tgtWord << endl;
         if (jawsNet[tgtWord].size() > 0) {
           cntPolysemousVerbsProcessedInVtFoundInJaws++;
           if (jawsNet[tgtWord].find(id)!=jawsNet[tgtWord].end()) {
             cntPolysemousVerbsProcessedInVtAgreeWithJaws++;
           } else {
             cntType4++;
             cout <<id <<":Error Type 4 : "<< tgtWord <<" exists in jaws not in " << id << " : ";
             for (set<string>::iterator itIdent = vtNetIdIdent[id].begin(); itIdent!= vtNetIdIdent[id].end(); itIdent++) {
               cout << *itIdent << ", " ;
             }
             cout << ". "<< endl;
             cout << id <<" : " << glosses[id] << endl;
             cout << ", where they prefer : ";
             for (set<string>::iterator itIdent = jawsNetIdIdent[id].begin(); itIdent!= jawsNetIdIdent[id].end(); itIdent++) {
               cout << *itIdent << ", " ;
             }
         
             cout << ". "<< endl;
             cout << ", but in " ;
             for (set<string>::iterator itId = jawsNet[tgtWord].begin(); itId!= jawsNet[tgtWord].end(); itId++) {
               cout << *itId << ": " ;
               for (set<string>::iterator itIdent = jawsNetIdIdent[*itId].begin(); itIdent!= jawsNetIdIdent[*itId].end(); itIdent++) {
                  cout << *itIdent << ", " ;
               }
               cout << ". "<< endl;
               cout <<  *itId <<" : " << glosses[*itId] << endl;
             }
             cout << "." <<endl;

             cout << "------"<< endl;
           }
         } else {
           cntType3++;
           cout << id <<":Error Type 3 : "<< tgtWord<<" does not exist in jaws." << endl;
           cout << "In Vt :  "<< endl;
           cout << glosses[id] << endl;
           cout << ", where they prefer : ";
           for (set<string>::iterator itIdent = jawsNetIdIdent[id].begin(); itIdent!= jawsNetIdIdent[id].end(); itIdent++) {
             cout << *itIdent << ", " ;
           }
           cout << ". "<< endl;
           cout << "------"<< endl;
         }
      }
      // count common processed ids
      if (vtNetIdIdent[id].size() > 0) {
         cntCommonPolysemousId+=nbInstances;
      }
    } // end if id is polysemous
    candidates.clear();
    nbInstances=0;
  } // end endElement SYNSET
  
}
