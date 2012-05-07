#include "JawsEvaluatorHandler.hpp"
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "Paths.hpp"
#include "Tools.hpp"

using namespace xercesc;
using namespace std;



JawsEvaluatorHandler::JawsEvaluatorHandler( set<string>& litList, set<string>& _polysemousIdsList, map<string, set<string> >& _vtNet, map<string, set<string> >& _vtNetIdIdent, string& datafile) :
  litList(litList), polysemousIdsList(_polysemousIdsList), vtNet(_vtNet), vtNetIdIdent(_vtNetIdIdent),
  nbSynsets(0),   nbInstances(0),   cntCommonPolysemousId(0), 
  cntPolysemousNounsProcessedInJaws(0),
  cntPolysemousNounsProcessedInJawsFoundInVt(0),
  cntPolysemousNounsProcessedInJawsAgreeWithVt(0),
  cntPolysemousNounsProcessedInVt(0),
  cntPolysemousNounsProcessedInVtFoundInJaws(0),
  cntPolysemousNounsProcessedInVtAgreeWithJaws(0),  
  nbOriginalLit(0),
  cntType1(0),
  cntType2(0),
  cntType3(0),
  cntType4(0) {


  candidates = map<string, set<string > >();
  processingTypes = map<string, string>();
  XMLTransService* const  theService =
    XMLPlatformUtils::fgTransService;

  XMLTransService::Codes  theCode;

  theTranscoder =
    theService->makeNewTranscoderFor(
                                         "utf-8",
                                         theCode,
                                         8192);

  ifstream idss(datafile.c_str());
  string s;
  while (getline(idss, s) ) {         
    glosses[s.substr(0, s.find(' '))]=s.substr(s.find(' '));
  }
  idss.close();

}

JawsEvaluatorHandler::~JawsEvaluatorHandler() {
  delete theTranscoder;
}


string JawsEvaluatorHandler::_transcode(const XMLCh* const chars) {
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

bool JawsEvaluatorHandler::checkAttr(const Attributes &           attrs, string key, string value ) {
  XMLCh * _key = XMLString::transcode(key.c_str());
  bool res =_transcode(attrs.getValue(_key)).compare(value)==0 ;
  XMLString::release(&_key);
  return (res);
}

string JawsEvaluatorHandler::getAttrValue(const Attributes &           attrs, string value) {
  XMLCh * _key =  XMLString::transcode(value.c_str());
  if (attrs.getValue(_key)==NULL) {
    XMLString::release(&_key);
    return "";
  }
  string res = _transcode(attrs.getValue(_key));
  XMLString::release(&_key);
  return   res;
}

void JawsEvaluatorHandler::startElement(const XMLCh *const              uri,
                                             const XMLCh *const            localname,
                                             const XMLCh *const            qname,
                                             const Attributes &           attrs) {
  //  cerr << "START : " << _transcode(qname) << endl;
  if(_transcode(qname).compare("SYNSET")==0) {
    nbSynsets++;
    if (nbSynsets%10000==0) {
      cerr << "nbSynset : " << nbSynsets << endl;
    }
    id=getAttrValue(attrs, "id");
  } else if(_transcode(qname).compare("INSTANCE")==0) {    
    string originals=getAttrValue(attrs, "original");
    while(originals.find(",")!=string::npos) {
      originalList.insert(originals.substr(0, originals.find(",")));
      originals=originals.substr(originals.find(",")+1);
    }

  } else if(_transcode(qname).compare("CANDIDATES")==0) {
    processed=getAttrValue(attrs, "processed"); 
    originalSrc=getAttrValue(attrs, "original");
  }
  
}

void JawsEvaluatorHandler::characters(const XMLCh *const chars, const XMLSize_t length)  {
  tmpString = _transcode(chars);
}

void JawsEvaluatorHandler::endElement(const XMLCh *const uri,
                                          const XMLCh *const localname,
                                          const XMLCh *const qname) {
  if (_transcode(qname).compare("INSTANCE")==0) {
    for (set<string>::iterator itOrig = originalList.begin(); itOrig !=originalList.end(); itOrig++) {
      string original = *itOrig; 
      if (litList.find(original)!=litList.end()) {
         cntPolysemousNounsProcessedInJaws++;
         nbInstances++;
         while (tmpString.find('_')!=string::npos) {
           tmpString=tmpString.replace(tmpString.find('_'), 1, " ");
         }
         tmpString=tolower(tmpString);
         cerr << "Testing : " << tmpString << " : " << id << endl;
         for (set<string>::iterator ittest = vtNet[tmpString].begin(); ittest !=vtNet[tmpString].end() ; ittest++) {
         cerr <<         "Inside : " <<*ittest<< endl;
         }
      jawsNet[tmpString].insert(id);
      jawsNetIdIdent[id].insert(tmpString);
      if (vtNetIdIdent[id].size() > 0) {
         cntPolysemousNounsProcessedInJawsFoundInVt++;
         if (vtNet[tmpString].find(id)!=vtNet[tmpString].end()) {
           cntPolysemousNounsProcessedInJawsAgreeWithVt++;
         } else {
           cntType2++;
           cout <<":Error Type 2 : "<< tmpString<<"("<<id << " : " << processingTypes[original]<< " : " << original <<": ";
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
             if (tmpString.compare(*itIdent)==0) {
               cout << "MATCH point" << endl;
             }
           }
         
           cout << ". "<< endl;
           cout << ", but in " ;
           for (set<string>::iterator itId = vtNet[tmpString].begin(); itId!= vtNet[tmpString].end(); itId++) {
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
         cout << id <<":Error Type 1 : '"<< id <<"'("<< tmpString<< " : " << processingTypes[original]<< " : " << original <<":";
         for (set<string>::iterator itCand = candidates[original].begin(); itCand!= candidates[original].end(); itCand++) {
           cout << *itCand << ", ";
         }
         cout << ") does not exist in vt." << endl;
         cout << "In Jaws :  "<< endl;
         for (set<string>::iterator itId = jawsNet[tmpString].begin(); itId != jawsNet[tmpString].end(); itId++) {
           cout << *itId <<" : " << glosses[*itId] << endl;
         }
         assert(vtNetIdIdent[id].size()==0);
         cout << ". "<< endl;
         cout << "------"<< endl;
      }
    }
    } // end itOrig
    originalList.clear();
  } else if (_transcode(qname).compare("CANDIDATES")==0) {
    if (litList.find(originalSrc)!=litList.end()) {
      cerr << "o:" << originalSrc << endl;
      nbOriginalLit++;    
    }
    processingTypes[originalSrc]=processed;    
  } else if (_transcode(qname).compare("CANDIDATE")==0) {
    candidates[originalSrc].insert(tmpString);    
  } else if (_transcode(qname).compare("SYNSET")==0) {
    if (polysemousIdsList.find(id)!=polysemousIdsList.end() ) {      
      for (set<string>::iterator it = vtNetIdIdent[id].begin() ; it != vtNetIdIdent[id].end(); it++ ) {
         cntPolysemousNounsProcessedInVt++;
         if (jawsNetIdIdent[id].size() > 0) {
           cntPolysemousNounsProcessedInVtFoundInJaws++;
           if (jawsNet[*it].find(id)!=jawsNet[*it].end()) {
             cntPolysemousNounsProcessedInVtAgreeWithJaws++;
           } else {
             cntType4++;
             cout <<id <<":Error Type 4 : "<< *it<<" exists in jaws not in " << id << " : ";
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
             for (set<string>::iterator itId = jawsNet[*it].begin(); itId!= jawsNet[*it].end(); itId++) {
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
           cout << id <<":Error Type 3 : "<< *it<<" does not exist in jaws." << endl;
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
    processingTypes.clear();
    nbInstances=0;
  } // end endElement SYNSET
  
}
