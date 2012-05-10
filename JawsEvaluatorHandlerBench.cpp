#include "JawsEvaluatorHandlerBench.hpp"
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "Paths.hpp"

using namespace xercesc;
using namespace std;



JawsEvaluatorBenchHandler::JawsEvaluatorBenchHandler(map<string, int>& _bcsbase, set<string>& litList, set<string>& _polysemousIdsList, map<string, set<string> >& _vtNet, map<string, set<string> >& _vtNetIdIdent, string& datafile, BCSMode _bcsmode) :
  JawsEvaluatorHandler(litList, _polysemousIdsList,  _vtNet,_vtNetIdIdent,datafile), bcsbase(_bcsbase), bcsmode(_bcsmode) {

}

JawsEvaluatorBenchHandler::~JawsEvaluatorBenchHandler() {
}

void JawsEvaluatorBenchHandler::endElement(const XMLCh *const uri,
				      const XMLCh *const localname,
				      const XMLCh *const qname) {
  if (_transcode(qname).compare("INSTANCE")==0) {
    for (set<string>::iterator itOrig = originalList.begin(); itOrig !=originalList.end(); itOrig++) {
      string original = *itOrig; 
      if (bcsbase[id]==bcsmode) {
	//      if (litList.find(original)!=litList.end()) {
	cntPolysemousNounsProcessedInJaws++;
	nbInstances++;
	while (tmpString.find('_')!=string::npos) {
	  tmpString=tmpString.replace(tmpString.find('_'), 1, " ");
	}
      jawsNet[tmpString].insert(id);
      jawsNetIdIdent[id].insert(tmpString);
      if (vtNet[tmpString].size() > 0) {
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
	assert(vtNet[tmpString].size()==0);
	cout << ". "<< endl;
	cout << "------"<< endl;
      }
    }
    } // end itOrig
    originalList.clear();
  } else if (_transcode(qname).compare("CANDIDATES")==0) {
      if (bcsbase[id]==bcsmode) {
	//    if (litList.find(originalSrc)!=litList.end()) {
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
	if (jawsNet[*it].size() > 0) {
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
