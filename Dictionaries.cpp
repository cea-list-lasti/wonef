#include "Dictionaries.hpp"
#include "Paths.hpp"

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <iostream>
#include <fstream>

std::map<std::string, std::string> Dictionaries::definition;

Dictionaries::Dictionaries(std::string pos) {

  std::set<std::string> dicfiles{EURADIC, WIKTIONARY};

  for (std::string dicfile : dicfiles) {
    std::cerr << "Opening "<< dicfile << std::endl;
    std::ifstream idss(dicfile, std::fstream::in);
    if (idss.fail()) {
      std::cerr << "Oops, " << dicfile << " doesn't exist. " << __FILE__ << ":" << __LINE__ << std::endl;
      exit(-1);
    }
    std::string s;

    while (getline(idss, s) ) {
      s=boost::regex_replace(s, boost::regex(" "), "_");

      std::vector<std::string> strs;
      boost::split(strs, s, boost::is_any_of(";"));

      std::string tgtWord = strs[0];
      std::string tgtPos = strs[1];
      std::string srcWord = strs[3];
      std::string srcPos = strs[4];

      // insert if Wiktionary or if ELDA and valid part-of-speech
      if (dicfile == WIKTIONARY || validPos(pos, srcPos) || validPos(pos, tgtPos)) {
        translations[srcWord].insert(tgtWord);
      }

      // store definition for Wiktionary
      if (strs.size() >= 6) {
        definition[tgtWord]=strs[5];
      }
    }
    idss.close();
  }
  std::cerr << "dictionaries opened" << std::endl;
}

bool Dictionaries::validPos(std::string wantedPos, std::string candidatePos) {
  if (wantedPos == "noun") { return candidatePos == "S" || candidatePos == "NP"; }
  else if (wantedPos == "verb") { return candidatePos == "V"; }
  else if (wantedPos == "adj") { return candidatePos == "J"; }
  else if (wantedPos == "adv") { return candidatePos == "D"; }
  else { std::cerr <<  "Oops, pos " << wantedPos << " doesn't exist." << __FILE__ << ":" << __LINE__ ; exit(-1); }
}


