#include <libxml++/libxml++.h>

#include <string>
#include <sstream>
#include <set>
#include <map>
#include <iostream>
#include <fstream>

using namespace std;

class WolfOneHandler : public xmlpp::SaxParser {

public:

  WolfOneHandler(map<string, set<string> >& wolfNet,
	      map<string, set<string> >& wolfNetIdIdent,
	      string _pos);

  void on_start_element(const std::string& name, const xmlpp::SaxParser::AttributeList& properties);
  void on_characters(const std::string& characters);
  void on_end_element(const std::string &name);

private:
  uint nbSynsets;
  string id;
  string PartOfSpeech;
  string literal;
  stringstream tmpString;
  string pos;
  map<string, set<string> >& wolfNet;
  map<string, set<string> >& wolfNetIdIdent;
  std::map<std::string, std::string> sensemap;
};
