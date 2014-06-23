#include "BCSBaseHandler.hpp"

#include "Tools.hpp"
#include "Paths.hpp"

#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>

BcsbaseHandler::BcsbaseHandler(std::map<std::string, int >& _bcsbase, std::map<int, int>& _BCSCount, std::string _pos) :
  nbSynsets(0), pos(_pos), bcsbase(_bcsbase), BCSCount(_BCSCount) {
  sensemap = loadSensemap(pos);
  BCSCount = { {1, 0}, {2, 0}, {3, 0} };
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
    if((pos == "noun" && PartOfSpeech == "n")
        || (pos == "verb" && PartOfSpeech == "v")
        || (pos == "adj" && PartOfSpeech == "a")
        || (pos == "adv" && PartOfSpeech == "b")) {
      int bcs;
      tmpString >> bcs;
      bcsbase[sensemap[id]] = bcs;
      BCSCount[bcs]++;
    }
  }
}
