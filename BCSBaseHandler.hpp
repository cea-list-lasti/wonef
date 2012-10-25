#include <libxml++/libxml++.h>
#include <string>
#include <sstream>
#include <set>
#include <map>
#include <iostream>
#include <fstream>

using namespace std;

class BcsbaseHandler : public xmlpp::SaxParser {

public:

  BcsbaseHandler(map<string, int >& bcsbase, map<int, int>& BCSCount, string _pos);

  void on_start_element(const std::string& name, const xmlpp::SaxParser::AttributeList& properties) override;
  void on_characters(const std::string& characters) override;
  void on_end_element(const std::string &name) override;

private :
  uint nbSynsets;
  string id;
  string PartOfSpeech;
  string literal;
  stringstream tmpString;
  string pos;
  map<string, int >& bcsbase;
  map<int, int >& BCSCount;

  map<string, string> sensemap;
};
