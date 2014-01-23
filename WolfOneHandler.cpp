#include "WolfOneHandler.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

WolfOneHandler::WolfOneHandler(map<string, set<string> >& _wolfNet,
    map<string, set<string> >& _wolfNetIdIdent,
    string _pos) :
  nbSynsets(0), pos(_pos),
  wolfNet(_wolfNet), wolfNetIdIdent(_wolfNetIdIdent) {
}

void WolfOneHandler::on_start_element(const std::string& name, const xmlpp::SaxParser::AttributeList&) {
  tmpString.clear();
  tmpString.str(std::string());

  if(name == "SYNSET") {
    nbSynsets++;
  }
}

void WolfOneHandler::on_characters(const std::string& characters) {
    tmpString << characters;
}

void WolfOneHandler::on_end_element(const std::string &name) {
  if (name == "ID") {
    id = tmpString.str().substr(7, 8);
  } else if (name == "POS") {
    PartOfSpeech = tmpString.str();
  } else if (name == "LITERAL") {
    std::string literal = tmpString.str();
    if (literal != "_EMPTY_") {
      if ((pos == "noun" && PartOfSpeech == "n")
          || (pos == "verb" && PartOfSpeech == "v")
          || (pos == "adj" && PartOfSpeech == "a")
          || (pos == "adv" && PartOfSpeech == "b")) {
        //cerr << "INSERT : " << literal << " -> " << id << endl;
        wolfNet[literal].insert(id);
        wolfNetIdIdent[id].insert(literal);
      }
    }
  }

}
