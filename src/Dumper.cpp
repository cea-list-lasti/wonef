#include "tools.h"
#include "Dumper.hpp"
#include "Loader.hpp"
#include <iostream>
#include <fstream>
#include <sstream>



DumperModule::DumperModule(std::string _datafile) {
  datafile = _datafile;
}

void DumperModule::dump(WORDNET::WordNet& wn) {
  std::ofstream ofs(datafile.c_str(), std::ios_base::out | std::ios_base::trunc );
  ofs << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
  ofs << "<JAWS>" << std::endl;
  for (std::map<std::string, WORDNET::WordNetEntry>::iterator itwn = wn.begin(); itwn !=wn.end(); itwn++) {
    ofs << "\t<SYNSET id=\""<< itwn->first <<"\">" << std::endl;
    ofs << "\t\t<ORIGINALDEF>" << "<![CDATA[" << itwn->second.def << "]]>" <<"</ORIGINALDEF>"<<std::endl;
    /*    if (itwn->second.newdef!="") {
      ofs << "\t\t<NEWDEF>" << itwn->second.newdef <<"</NEWDEF>"<<std::endl; 
      }*/

    for (std::map<std::string, WORDNET::TgtCandidates>::iterator itwne = itwn->second.frenchCandidates.begin(); itwne !=itwn->second.frenchCandidates.end(); itwne++) {
      std::string escaped_translation = escape_xml(itwne->first);
      //      ofs << "\t\t<ORIGINAL>" << itwne->first << "</ORIGINAL>" << std::endl;
      ofs << "\t\t<CANDIDATES original=\"" << itwne->first
	  << "\" formerElected=\"" << itwne->second.formerElected << "\">" <<std::endl;
      for (std::map<std::string, int>::iterator itCand = itwne->second.cand.begin(); itCand !=itwne->second.cand.end(); itCand++) {	
	ofs << "\t\t\t<CANDIDATE score=\""<< itCand->second <<"\">" << itCand->first << "</CANDIDATE>"<< std::endl;
      }
      ofs << "\t\t</CANDIDATES>" <<std::endl; 
    }  
    for (std::map<std::string, std::set<WORDNET::TranslationInfos> >::iterator itwne = itwn->second.frenchSynset.begin(); itwne !=itwn->second.frenchSynset.end(); itwne++) {	
      std::string escaped_translation = escape_xml(itwne->first);
      //  ofs << "\t\t<INSTANCE original=\""<<itwne->second<<"\">" << itwne->first << "</INSTANCE>" <<std::endl;
      ofs << "\t\t<INSTANCES translation=\"" << escaped_translation << "\">" << std::endl;
      for (std::set<WORDNET::TranslationInfos>::iterator itSrc = itwne->second.begin(); itSrc != itwne->second.end(); itSrc++) {
	ofs << "\t\t\t<INSTANCE original=\"" << itSrc->original << "\"";
	ofs << " processed=\"" << itSrc->processed << "\"";
	ofs << " score=\"" << itSrc->score <<"\" />" << std::endl;
      }
      ofs << "\t\t</INSTANCES>" << std::endl;
    }
    for (std::map<std::string, std::set<WORDNET::TranslationInfos> >::iterator itwne = itwn->second.frenchScore.begin(); itwne !=itwn->second.frenchScore.end(); itwne++) {	
      std::string escaped_translation = escape_xml(itwne->first);
      ofs << "\t\t<SCORES translation=\"" << escaped_translation << "\">" << std::endl;
      for (std::set<WORDNET::TranslationInfos>::iterator itSrc = itwne->second.begin(); itSrc != itwne->second.end(); itSrc++) {
	ofs << "\t\t\t<SCORE original=\"" << itSrc->original << "\"";
	ofs << " processed=\"" << itSrc->processed << "\"";
	ofs << " score=\"" << itSrc->score <<"\" />" << std::endl;
      }
      ofs << "\t\t</SCORES>" << std::endl;
    }

    for (std::set<std::string>::iterator itH = itwn->second.hypers.begin(); itH != itwn->second.hypers.end(); itH++) {
      ofs << "\t\t<HYPER>" << *itH << "</HYPER>" <<std::endl;
    }
    for (std::set<std::string>::iterator itH = itwn->second.hypos.begin(); itH != itwn->second.hypos.end(); itH++) {
      ofs << "\t\t<HYPO>" << *itH << "</HYPO>" <<std::endl;
    }

    for (std::set<std::string>::iterator itH = itwn->second.meros.begin(); itH != itwn->second.meros.end(); itH++) {
      ofs << "\t\t<MERO>" << *itH << "</MERO>" <<std::endl;
    }

    for (std::set<std::string>::iterator itH = itwn->second.holos.begin(); itH != itwn->second.holos.end(); itH++) {
      ofs << "\t\t<HOLO>" << *itH << "</HOLO>" <<std::endl;
    }
    ofs << "\t</SYNSET>" << std::endl;
  }
    ofs << "</JAWS>" << std::endl;
  ofs.close();
}
