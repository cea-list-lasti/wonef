#include <libxml++/libxml++.h>

#include <string>
#include <set>
#include <map>
#include <iostream>
#include <fstream>

using namespace std;

class WolfHandler : public xmlpp::SaxParser {

public:

  WolfHandler(map<string, set<string> >* wolfNet,
	      map<string, set<string> >* wolfNetIdIdent,
	      string _pos);

  void on_start_element(const std::string& name, const xmlpp::SaxParser::AttributeList& properties) override;
  void on_characters(const std::string& characters) override;
  void on_end_element(const std::string &name) override;

private:
  uint nbSynsets;
  string id;
  string PartOfSpeech;
  string literal;
  string tmpString;
  string pos;
  map<string, set<string> >* wolfNet;
  map<string, set<string> >* wolfNetIdIdent;
  std::map<std::string, std::string> sensemap;
};
