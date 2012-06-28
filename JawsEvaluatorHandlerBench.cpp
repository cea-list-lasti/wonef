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

  if (bcsbase[id]==bcsmode && _transcode(qname).compare("INSTANCES")==0) {

    // checking if the translation comes from polysemous source nouns
    bool polysemous = false;
    for (set<string>::iterator itOrig = originalsList.begin();
	 itOrig != originalsList.end(); itOrig++){
      if (litList.find(*itOrig)!=litList.end()) {
	polysemous = true;
      }
    }

    // counting nouns in JAWS
    nbNounsInJaws++;
    if (polysemous == true) {
      nbPolysemousNounsInJaws++;
    }

    // counting nouns both in JAWS and VT
    if (vtNet[translation].size() > 0) {
      nbNounsInJawsAndVt++;
      if (polysemous == true) {
	nbPolysemousNounsInJawsAndVt++;
      }

      // counting nouns in the same synset in JAWS and VT
      if (vtNet[translation].find(id)!=vtNet[translation].end()) {
	nbNounsInJawsAgreeWithVt++;
	if (polysemous == true) {
	  nbPolysemousNounsInJawsAgreeWithVt++;
	}
      }
    }

    originalsList.clear();

  } else if (_transcode(qname).compare("INSTANCE")==0) {
      if (bcsbase[id]==bcsmode) {
	//      if (litList.find(original)!=litList.end()) {
	cntPolysemousNounsProcessedInJaws++;
	nbInstances++;
	while (translation.find('_')!=string::npos) {
	  translation=translation.replace(translation.find('_'), 1, " ");
	}
      jawsNet[translation].insert(id);
      jawsNetIdIdent[id].insert(translation);
      if (vtNet[translation].size() > 0) {
	cntPolysemousNounsProcessedInJawsFoundInVt++;
	if (vtNet[translation].find(id)!=vtNet[translation].end()) {
	  cntPolysemousNounsProcessedInJawsAgreeWithVt++;
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
	cout << id <<":Error Type 1 : '"<< id <<"'("<< translation<< " : " << processed << " : " << original <<":";
	for (set<string>::iterator itCand = candidates[original].begin(); itCand!= candidates[original].end(); itCand++) {
	  cout << *itCand << ", ";
	}
	cout << ") does not exist in vt." << endl;
	cout << "In Jaws :  "<< endl;
	for (set<string>::iterator itId = jawsNet[translation].begin(); itId != jawsNet[translation].end(); itId++) {
	  cout << *itId <<" : " << glosses[*itId] << endl;
	}
	assert(vtNet[translation].size()==0);
	cout << ". "<< endl;
	cout << "------"<< endl;
      }
    }

  } else if (_transcode(qname).compare("CANDIDATES")==0) {
      if (bcsbase[id]==bcsmode) {
	//    if (litList.find(originalSrc)!=litList.end()) {
      cerr << "o:" << originalSrc << endl;
      nbOriginalLit++;    
    }

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
    nbInstances=0;
  } // end endElement SYNSET
  
}
