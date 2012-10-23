#include "BCSBaseHandler.hpp"

#include "Tools.hpp"
#include "Paths.hpp"

#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>

BcsbaseHandler::BcsbaseHandler(map<string, int >& _bcsbase, map<int, int>& _BCSCount, string _pos) :
  nbSynsets(0), pos(_pos), bcsbase(_bcsbase), BCSCount(_BCSCount) {
  sensemap = loadSensemap(pos);
}

void BcsbaseHandler::on_start_element(const std::string& name,
    const xmlpp::SaxParser::AttributeList& /* properties */) {
  if(name == "SYNSET") {
    nbSynsets++;
  } else if(name == "SENSE") {
    literal = tmpString;
  }
}

void BcsbaseHandler::on_characters(const std::string& characters) {
    tmpString = characters;
}

void BcsbaseHandler::on_end_element(const std::string &name) {
  if (name == "ID") {
    id = tmpString.substr(6, 8);
  } else if (name == "POS") {
    PartOfSpeech = tmpString;
  } else if (name == "BCS") {
    if((pos == "noun" && PartOfSpeech.compare("n") == 0)
      || (pos == "verb" && PartOfSpeech.compare("v") == 0)
      || (pos == "adj" && PartOfSpeech.compare("a") == 0)) {
      stringstream ss;
      ss << tmpString;
      int bcs;
      ss >> bcs;
      bcsbase[sensemap[id]] = bcs;
      BCSCount[bcs]++;
    }
  }
}
