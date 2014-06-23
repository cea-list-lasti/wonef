#include <string>
#include <set>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include "WordNet.hpp"
#include <boost/regex.hpp>


class EwnLoader {

public:
  EwnLoader(std::map<std::string, std::set<std::string>>& ewnNet,
      std::map<std::string, std::set<std::string>>& ewnNetIdIdent,
      std::string& _filepath, std::map<std::string, std::set<std::string> >& _mapping);

  void load();

protected:
  std::map<std::string, std::set<std::string> >& ewnNet;
  std::map<std::string, std::set<std::string> >& ewnNetIdIdent;
  std::string filepath;
  uint nbSynsets;
  std::map<std::string, std::set<std::string> >& mapping;

};
