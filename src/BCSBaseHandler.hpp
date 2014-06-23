#include <libxml++/libxml++.h>
#include <string>
#include <sstream>
#include <set>
#include <map>
#include <iostream>
#include <fstream>

class BcsbaseHandler : public xmlpp::SaxParser {

public:

  BcsbaseHandler(std::map<std::string, int >& bcsbase, std::map<int, int>& BCSCount, std::string _pos);

  void on_start_element(const std::string& name, const xmlpp::SaxParser::AttributeList& properties);
  void on_characters(const std::string& characters);
  void on_end_element(const std::string &name);

private :
  uint nbSynsets;
  std::string id;
  std::string PartOfSpeech;
  std::string literal;
  std::stringstream tmpString;
  std::string pos;
  std::map<std::string, int >& bcsbase;
  std::map<int, int >& BCSCount;

  std::map<std::string, std::string> sensemap;
};
