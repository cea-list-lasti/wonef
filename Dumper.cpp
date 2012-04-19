# include "Dumper.hpp"
#include "Loader.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;


DumperModule::DumperModule(string _datafile, string _indexfile) {
  datafile = _datafile;
  indexfile = _indexfile;  
}


DumperModule::~DumperModule() {
}

void DumperModule::dump(WORDNET::WordNet& wn, bool verbose) {
  printData(wn, verbose);
  printUnsolved(wn);
  //  printIndex(verbose);
}



void DumperModule::printIndex(bool verbose) {
  ofstream indexOfs(indexfile.c_str());  
  for (map<string, set<string> >::iterator it = indexTgt.begin(); it!=indexTgt.end(); it++) {
    if (it->first !="" ) {
      stringstream ss;
      ss << it->first << " " << it->second.size() << " " ;
      for (set<string>::iterator it2 = it->second.begin() ; it2!=it->second.end(); it2++) {
	ss << *it2 << " " ;
      }
      indexOfs << ss.str() << endl;
    }
  }
  indexOfs.close(); 
}



void DumperModule::printData(WORDNET::WordNet& wn, bool verbose) {
  cerr << "Writing in " << datafile << endl;
  ofstream ofs(datafile.c_str(), ios_base::out | ios_base::trunc );
  ofs << "<JAWS pos=\"noun\" >" << endl;
  for (map<string, WORDNET::WordNetEntry>::iterator itwn = wn.begin(); itwn !=wn.end(); itwn++) {
    ofs << "\t<SYNSET id=\""<< itwn->first <<"\" >" << endl;
    ofs << "\t\t<ORIGINALDEF>" << itwn->second.def <<"</ORIGINALDEF>"<<endl; 
    /*    if (itwn->second.newdef!="") {
      ofs << "\t\t<NEWDEF>" << itwn->second.newdef <<"</NEWDEF>"<<endl; 
      }*/

    for (map<string, WORDNET::TgtCandidates>::iterator itwne = itwn->second.frenchCandidates.begin(); itwne !=itwn->second.frenchCandidates.end(); itwne++) {	
      //      ofs << "\t\t<ORIGINAL>" << itwne->first << "</ORIGINAL>" << endl;
      ofs << "\t\t<CANDIDATES original=\""<<itwne->first <<"\" processed=\""<< itwne->second.processed <<"\" formerElected=\"" << itwne->second.formerElected << "\" >" <<endl;
      for (map<string, int>::iterator itCand = itwne->second.cand.begin(); itCand !=itwne->second.cand.end(); itCand++) {	
	ofs << "\t\t\t<CANDIDATE score=\""<< itCand->second <<"\">" << itCand->first << "</CANDIDATE>"<< endl;
      }
      ofs << "\t\t</CANDIDATES>" <<endl; 
    }  
    for (map<string, set<string> >::iterator itwne = itwn->second.frenchSynset.begin(); itwne !=itwn->second.frenchSynset.end(); itwne++) {	
      //  ofs << "\t\t<INSTANCE original=\""<<itwne->second<<"\">" << itwne->first << "</INSTANCE>" <<endl;
      ofs << "\t\t<INSTANCE original=\"";
      for (set<string>::iterator itSrc = itwne->second.begin(); itSrc != itwne->second.end(); itSrc++) {
	ofs << *itSrc << ",";
      }
      ofs <<"\">" << itwne->first << "</INSTANCE>" <<endl;
    }

    for (set<string>::iterator itH = itwn->second.hypers.begin(); itH != itwn->second.hypers.end(); itH++) {
      ofs << "\t\t<HYPER>" << *itH << "</HYPER>" <<endl;
    }
    for (set<string>::iterator itH = itwn->second.hypos.begin(); itH != itwn->second.hypos.end(); itH++) {
      ofs << "\t\t<HYPO>" << *itH << "</HYPO>" <<endl;
    }

    for (set<string>::iterator itH = itwn->second.meros.begin(); itH != itwn->second.meros.end(); itH++) {
      ofs << "\t\t<MERO>" << *itH << "</MERO>" <<endl;
    }

    for (set<string>::iterator itH = itwn->second.holos.begin(); itH != itwn->second.holos.end(); itH++) {
      ofs << "\t\t<HOLO>" << *itH << "</HOLO>" <<endl;
    }
    ofs << "\t</SYNSET>" << endl;
  }
    ofs << "</JAWS>" << endl;
  ofs.close();
}




void DumperModule::printUnsolved(WORDNET::WordNet& wn) {
  cerr << "Writing in " << datafile << endl;
  stringstream datafiless;
  datafiless << datafile << ".unsolved" ; 
  ofstream ofs(datafiless.str().c_str(), ios_base::out | ios_base::trunc );
  ofs << "<JAWS pos=\"noun\" >" << endl;
  for (map<string, WORDNET::WordNetEntry>::iterator itwn = wn.begin(); itwn !=wn.end(); itwn++) {
    // SKIP SOLVED SYNSET
    bool tosolve = false;
    for (map<string, WORDNET::TgtCandidates>::iterator itwne = itwn->second.frenchCandidates.begin(); itwne !=itwn->second.frenchCandidates.end(); itwne++) {	
      if (itwne->second.cand.size()>0) {
	tosolve = true;
	break;
      } 
    }
  
    if (itwn->second.frenchSynset.size() >= itwn->second.frenchCandidates.size() && tosolve) {
      continue;
    }
    //    cerr << "\t<SYNSET id=\""<< itwn->first <<"\" >" << endl;
    ofs << "\t<SYNSET id=\""<< itwn->first <<"\" >" << endl;
    ofs << "\t\t<ORIGINALDEF>" << itwn->second.def <<"</ORIGINALDEF>"<<endl; 
    /*    if (itwn->second.newdef!="") {
      ofs << "\t\t<NEWDEF>" << itwn->second.newdef <<"</NEWDEF>"<<endl; 
      }*/

    for (map<string, WORDNET::TgtCandidates>::iterator itwne = itwn->second.frenchCandidates.begin(); itwne !=itwn->second.frenchCandidates.end(); itwne++) {	
      //      ofs << "\t\t<ORIGINAL>" << itwne->first << "</ORIGINAL>" << endl;
      ofs << "\t\t<CANDIDATES original=\""<<itwne->first <<"\" processed=\""<< itwne->second.processed <<"\" formerElected=\"" << itwne->second.formerElected << "\" >" <<endl;
      for (map<string, int>::iterator itCand = itwne->second.cand.begin(); itCand !=itwne->second.cand.end(); itCand++) {	
	ofs << "\t\t\t<CANDIDATE score=\""<< itCand->second <<"\">" << itCand->first << "</CANDIDATE>"<< endl;
      }
      ofs << "\t\t</CANDIDATES>" <<endl; 
    }  
    for (map<string, set<string> >::iterator itwne = itwn->second.frenchSynset.begin(); itwne !=itwn->second.frenchSynset.end(); itwne++) {	
      ofs << "\t\t<INSTANCE original=\"";
      for (set<string>::iterator itSrc = itwne->second.begin(); itSrc != itwne->second.end(); itSrc++) {
	ofs << *itSrc << ",";
      }
      ofs <<"\">" << itwne->first << "</INSTANCE>" <<endl;
    }

    for (set<string>::iterator itH = itwn->second.hypers.begin(); itH != itwn->second.hypers.end(); itH++) {
      ofs << "\t\t<HYPER>" << *itH << "</HYPER>" <<endl;
    }
    for (set<string>::iterator itH = itwn->second.hypos.begin(); itH != itwn->second.hypos.end(); itH++) {
      ofs << "\t\t<HYPO>" << *itH << "</HYPO>" <<endl;
    }

    for (set<string>::iterator itH = itwn->second.meros.begin(); itH != itwn->second.meros.end(); itH++) {
      ofs << "\t\t<MERO>" << *itH << "</MERO>" <<endl;
    }

    for (set<string>::iterator itH = itwn->second.holos.begin(); itH != itwn->second.holos.end(); itH++) {
      ofs << "\t\t<HOLO>" << *itH << "</HOLO>" <<endl;
    }
    ofs << "\t</SYNSET>" << endl;
  }
    ofs << "</JAWS>" << endl;
  ofs.close();
}
