#ifndef GOLDHANDLER_HPP
#define GOLDHANDLER_HPP

#include <libxml++/libxml++.h>

#include <map>
#include <set>
#include <utility>
#include <string>
#include <sstream>

class GoldHandler : public xmlpp::SaxParser {

public:
  GoldHandler(std::map<std::string, std::set<std::string> >& _goldNet,
              std::map<std::string, std::set<std::string> >& _goldNetIdIdent,
              std::map<std::pair<std::string, std::string>, int>& _goldValue);

  void on_start_element(const std::string& name, const xmlpp::SaxParser::AttributeList& properties) override;
  void on_characters(const std::string& characters) override;
  void on_end_element(const std::string &name) override;

protected :
  uint nbSynsets;
  int valide;
  std::string id;
  std::stringstream tmpString;
  // map<frenchWord, set<ids> >
  std::map<std::string, std::set<std::string> >& goldNet;
  // map<id, set<frenchWords> >
  std::map<std::string, std::set<std::string> >& goldNetIdIdent;
  // map<pair<id, frenchWord>, value>
  std::map<std::pair<std::string, std::string>, int>& goldValue;

private:
  std::string get_attr(const xmlpp::SaxParser::AttributeList& attrs, std::string name);

};

#endif
