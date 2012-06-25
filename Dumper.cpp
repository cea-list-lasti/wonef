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

void DumperModule::dump(WORDNET::WordNet& wn) {
  printData(wn);
  printUnsolved(wn);
  //  printIndex();
}



void DumperModule::printIndex() {
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



void DumperModule::printData(WORDNET::WordNet& wn) {
  cerr << "Writing in " << datafile << endl;
  ofstream ofs(datafile.c_str(), ios_base::out | ios_base::trunc );
  if (datafile.find("noun")!=string::npos) {
    ofs << "<JAWS pos=\"noun\" >" << endl;
  } else if (datafile.find("verb")!=string::npos) {
    ofs << "<JAWS pos=\"verb\" >" << endl;
  }
  for (map<string, WORDNET::WordNetEntry>::iterator itwn = wn.begin(); itwn !=wn.end(); itwn++) {
    ofs << "\t<SYNSET id=\""<< itwn->first <<"\" >" << endl;
    ofs << "\t\t<ORIGINALDEF>" << itwn->second.def <<"</ORIGINALDEF>"<<endl; 
    /*    if (itwn->second.newdef!="") {
      ofs << "\t\t<NEWDEF>" << itwn->second.newdef <<"</NEWDEF>"<<endl; 
      }*/

    for (map<string, WORDNET::TgtCandidates>::iterator itwne = itwn->second.frenchCandidates.begin(); itwne !=itwn->second.frenchCandidates.end(); itwne++) {	
      //      ofs << "\t\t<ORIGINAL>" << itwne->first << "</ORIGINAL>" << endl;
      ofs << "\t\t<CANDIDATES original=\"" << itwne->first
	  << "\" formerElected=\"" << itwne->second.formerElected << "\" >" <<endl;
      for (map<string, int>::iterator itCand = itwne->second.cand.begin(); itCand !=itwne->second.cand.end(); itCand++) {	
	ofs << "\t\t\t<CANDIDATE score=\""<< itCand->second <<"\">" << itCand->first << "</CANDIDATE>"<< endl;
      }
      ofs << "\t\t</CANDIDATES>" <<endl; 
    }  
    for (map<string, std::set<WORDNET::TranslationInfos> >::iterator itwne = itwn->second.frenchSynset.begin(); itwne !=itwn->second.frenchSynset.end(); itwne++) {	
      //  ofs << "\t\t<INSTANCE original=\""<<itwne->second<<"\">" << itwne->first << "</INSTANCE>" <<endl;
      ofs << "\t\t<INSTANCES translation=\"" << itwne->first << "\" >" << endl;
      for (set<WORDNET::TranslationInfos>::iterator itSrc = itwne->second.begin(); itSrc != itwne->second.end(); itSrc++) {
	ofs << "\t\t\t<INSTANCE original=\"" << itSrc->original << "\"";
	ofs << " processed=\"" << itSrc->processed << "\"";
	ofs << " score=\"" << itSrc->score <<"\" />" << endl;
      }
      ofs << "\t\t</INSTANCES>" << endl;
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
  if (datafile.find("noun")!=string::npos) {
    ofs << "<JAWS pos=\"noun\" >" << endl;
  } else if (datafile.find("verb")!=string::npos) {
    ofs << "<JAWS pos=\"verb\" >" << endl;
  }
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
      ofs << "\t\t<CANDIDATES original=\""<<itwne->first
	  << "\" formerElected=\"" << itwne->second.formerElected << "\" >" <<endl;
      for (map<string, int>::iterator itCand = itwne->second.cand.begin(); itCand !=itwne->second.cand.end(); itCand++) {	
	ofs << "\t\t\t<CANDIDATE score=\""<< itCand->second <<"\">" << itCand->first << "</CANDIDATE>"<< endl;
      }
      ofs << "\t\t</CANDIDATES>" <<endl; 
    }  
    for (map<string, std::set<WORDNET::TranslationInfos> >::iterator itwne = itwn->second.frenchSynset.begin(); itwne !=itwn->second.frenchSynset.end(); itwne++) {	
      //  ofs << "\t\t<INSTANCE original=\""<<itwne->second<<"\">" << itwne->first << "</INSTANCE>" <<endl;
      ofs << "\t\t<INSTANCES translation=\"" << itwne->first << "\" >" << endl;
      for (set<WORDNET::TranslationInfos>::iterator itSrc = itwne->second.begin(); itSrc != itwne->second.end(); itSrc++) {
	ofs << "\t\t\t<INSTANCE original=\""<< itSrc->original << "\"";
	ofs << " processed=\"" << itSrc->processed << "\"";
	ofs << " score=\"" << itSrc->score <<"\" />" << endl;
      }
      ofs << "\t\t</INSTANCES>" << endl;
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
