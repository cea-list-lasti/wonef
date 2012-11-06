#include "../src/tools.h"
#include "Dumper.hpp"
#include "Loader.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;


DumperModule::DumperModule(string _datafile) {
  datafile = _datafile;
}

void DumperModule::dump(WORDNET::WordNet& wn) {
  cerr << "Writing in " << datafile << endl;
  ofstream ofs(datafile.c_str(), ios_base::out | ios_base::trunc );
  ofs << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
  if (datafile.find("noun")!=string::npos) {
    ofs << "<JAWS pos=\"noun\">" << endl;
  } else if (datafile.find("verb")!=string::npos) {
    ofs << "<JAWS pos=\"verb\">" << endl;
  } else if (datafile.find("adj")!=string::npos) {
    ofs << "<JAWS pos=\"adj\">" << endl;
  }
  for (map<string, WORDNET::WordNetEntry>::iterator itwn = wn.begin(); itwn !=wn.end(); itwn++) {
    ofs << "\t<SYNSET id=\""<< itwn->first <<"\">" << endl;
    ofs << "\t\t<ORIGINALDEF>" << "<![CDATA[" << itwn->second.def << "]]>" <<"</ORIGINALDEF>"<<endl;
    /*    if (itwn->second.newdef!="") {
      ofs << "\t\t<NEWDEF>" << itwn->second.newdef <<"</NEWDEF>"<<endl; 
      }*/

    for (map<string, WORDNET::TgtCandidates>::iterator itwne = itwn->second.frenchCandidates.begin(); itwne !=itwn->second.frenchCandidates.end(); itwne++) {
      std::string escaped_translation = escape_xml(itwne->first);
      //      ofs << "\t\t<ORIGINAL>" << itwne->first << "</ORIGINAL>" << endl;
      ofs << "\t\t<CANDIDATES original=\"" << itwne->first
	  << "\" formerElected=\"" << itwne->second.formerElected << "\">" <<endl;
      for (map<string, int>::iterator itCand = itwne->second.cand.begin(); itCand !=itwne->second.cand.end(); itCand++) {	
	ofs << "\t\t\t<CANDIDATE score=\""<< itCand->second <<"\">" << itCand->first << "</CANDIDATE>"<< endl;
      }
      ofs << "\t\t</CANDIDATES>" <<endl; 
    }  
    for (map<string, std::set<WORDNET::TranslationInfos> >::iterator itwne = itwn->second.frenchSynset.begin(); itwne !=itwn->second.frenchSynset.end(); itwne++) {	
      std::string escaped_translation = escape_xml(itwne->first);
      //  ofs << "\t\t<INSTANCE original=\""<<itwne->second<<"\">" << itwne->first << "</INSTANCE>" <<endl;
      ofs << "\t\t<INSTANCES translation=\"" << escaped_translation << "\">" << endl;
      for (set<WORDNET::TranslationInfos>::iterator itSrc = itwne->second.begin(); itSrc != itwne->second.end(); itSrc++) {
	ofs << "\t\t\t<INSTANCE original=\"" << itSrc->original << "\"";
	ofs << " processed=\"" << itSrc->processed << "\"";
	ofs << " score=\"" << itSrc->score <<"\" />" << endl;
      }
      ofs << "\t\t</INSTANCES>" << endl;
    }
    for (map<string, std::set<WORDNET::TranslationInfos> >::iterator itwne = itwn->second.frenchScore.begin(); itwne !=itwn->second.frenchScore.end(); itwne++) {	
      std::string escaped_translation = escape_xml(itwne->first);
      ofs << "\t\t<SCORES translation=\"" << escaped_translation << "\">" << endl;
      for (set<WORDNET::TranslationInfos>::iterator itSrc = itwne->second.begin(); itSrc != itwne->second.end(); itSrc++) {
	ofs << "\t\t\t<SCORE original=\"" << itSrc->original << "\"";
	ofs << " processed=\"" << itSrc->processed << "\"";
	ofs << " score=\"" << itSrc->score <<"\" />" << endl;
      }
      ofs << "\t\t</SCORES>" << endl;
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
