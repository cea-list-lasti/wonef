#include "GoldHandler.hpp"
#include "Tools.hpp"
#include <boost/lexical_cast.hpp>
#include <utility>

using namespace std;

GoldHandler::GoldHandler(std::map<std::string, std::set<std::string> >& _goldNet,
                         std::map<std::string, std::set<std::string> >& _goldNetIdIdent,
                         std::map<std::pair<std::string, std::string>, int>& _goldValue) :
  nbSynsets(0), goldNet(_goldNet), goldNetIdIdent(_goldNetIdIdent), goldValue(_goldValue)
{

}

void GoldHandler::on_characters(const std::string& characters) {
  tmpString << characters;
}

std::string GoldHandler::get_attr(const xmlpp::SaxParser::AttributeList& attrs, std::string name) {
  for(const xmlpp::SaxParser::Attribute& attr: attrs) {
    if (attr.name == name) return attr.value;
  }
  std::cout << "Asked for " << name << ". Exiting." << std::endl;
  exit(255);
}

void GoldHandler::on_start_element(const std::string& name,
    const xmlpp::SaxParser::AttributeList& attrs) {

  tmpString.clear();
  tmpString.str(std::string());

  if(name == "SYNSET") {
    nbSynsets++;
    id = get_attr(attrs, "id");
  } else if(name == "CANDIDATE") {
    string val = get_attr(attrs, "score");
    valide = boost::lexical_cast<int>(val);
  }

}

void GoldHandler::on_end_element(const std::string &name) {
  std::string translation = tmpString.str();
  if (name == "CANDIDATE") {
    if (valide == 1) {
      goldNet[translation].insert(id);
      goldNetIdIdent[id].insert(translation);

    }

    goldValue[std::make_pair(id, translation)] = valide;
  }

}
