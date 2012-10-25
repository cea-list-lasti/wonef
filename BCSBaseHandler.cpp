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
    literal = tmpString.str();
  }

  tmpString.clear();
  tmpString.str(std::string());
}

void BcsbaseHandler::on_characters(const std::string& characters) {
    tmpString << characters;
}

void BcsbaseHandler::on_end_element(const std::string &name) {

  if (name == "ID") {
    id = tmpString.str().substr(6, 8);
  } else if (name == "POS") {
    PartOfSpeech = tmpString.str();
  } else if (name == "BCS") {
    if((pos == "noun" && PartOfSpeech.compare("n") == 0)
        || (pos == "verb" && PartOfSpeech.compare("v") == 0)
        || (pos == "adj" && PartOfSpeech.compare("a") == 0)) {
      int bcs;
      tmpString >> bcs;
      bcsbase[sensemap[id]] = bcs;
      BCSCount[bcs]++;
    }
  }
}
