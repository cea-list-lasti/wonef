#include "WolfHandler.hpp"
#include "Tools.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

WolfHandler::WolfHandler(map<string, set<string> >& _wolfNet,
    map<string, set<string> >& _wolfNetIdIdent,
    string _pos) :
  nbSynsets(0), pos(_pos),
  wolfNet(_wolfNet), wolfNetIdIdent(_wolfNetIdIdent) {

  sensemap = loadSensemap(pos);
}

void WolfHandler::on_start_element(const std::string& name, const xmlpp::SaxParser::AttributeList&) {
  if(name == "SYNSET") {
    nbSynsets++;
  } else if(name == "SENSE") {
    literal = tmpString.str();
  }

  tmpString.clear();
  tmpString.str(std::string());

}

void WolfHandler::on_characters(const std::string& characters) {
    tmpString << characters;
}

void WolfHandler::on_end_element(const std::string &name) {
  if (name == "ID") {
    id = sensemap[tmpString.str().substr(6, 8)];
  } else if (name == "POS") {
    PartOfSpeech = tmpString.str();
  } else if (name == "LITERAL") {
    if ((pos == "noun" && PartOfSpeech == "n")
      || (pos == "verb" && PartOfSpeech == "v")
      || (pos == "adj" && PartOfSpeech == "a")
      || (pos == "adv" && PartOfSpeech == "b")) {
      // cerr << "INSERT : " << literal << " -> " << id << endl;
      wolfNet[literal].insert(id);
      wolfNetIdIdent[id].insert(literal);
    }
  }

}
