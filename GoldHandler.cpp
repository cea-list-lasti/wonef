#include "GoldHandler.hpp"
#include "Tools.hpp"
#include <boost/lexical_cast.hpp>

using namespace std;

GoldHandler::GoldHandler(std::map<std::string, std::set<std::string> >* _goldNet,
                         std::map<std::string, std::set<std::string> >* _goldNetIdIdent,
                         std::map<std::pair<std::string, std::string>, int>* _goldValue) {

  goldNet = _goldNet;
  goldNetIdIdent = _goldNetIdIdent;
  goldValue = _goldValue;
  nbSynsets = 0;
}

void GoldHandler::on_characters(const std::string& characters) {
  tmpString = characters;
}

std::string GoldHandler::get_attr(const xmlpp::SaxParser::AttributeList& attrs, std::string name) {
  for(const xmlpp::SaxParser::Attribute& attr: attrs) {
    if (attr.name == name) return attr.value;
  }
  exit(-1);
}

void GoldHandler::on_start_element(const std::string& name,
    const xmlpp::SaxParser::AttributeList& attrs) {
  if(name == "SYNSET") {
    nbSynsets++;
    id = get_attr(attrs, "id");
  } else if(name == "CANDIDATE") {
    string val = get_attr(attrs, "valide");
    valide = boost::lexical_cast<int>(val);
  }

}

void GoldHandler::on_end_element(const std::string &name) {
  if (name == "CANDIDATE") {
    if (valide == 1) {
      if (goldNet->find(tmpString) == goldNet->end()) {
        (*goldNet)[tmpString] = set<string>();
      }
      (*goldNet)[tmpString].insert(id);
      if (goldNetIdIdent->find(id) == goldNetIdIdent->end()) {
        goldNetIdIdent->insert(make_pair(id, set<string>()));
      }
      (*goldNetIdIdent)[id].insert(tmpString);
    }

    pair<string, string> pairSynsetWord = pair<string, string>(id, tmpString);
    (*goldValue)[pairSynsetWord] = valide;
  }

}
