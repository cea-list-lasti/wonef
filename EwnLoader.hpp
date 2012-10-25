
#include <string>
#include <set>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include "WordNet.hpp"
#include <boost/regex.hpp>


using namespace std;

class EwnLoader {

public:
  EwnLoader(map<string, set<string>>& ewnNet,
      map<string, set<string>>& ewnNetIdIdent,
      string& _filepath, map<string, set<string> >& _mapping);

  void load();

protected:
  map<string, set<string> >& ewnNet;
  map<string, set<string> >& ewnNetIdIdent;
  string filepath;
  uint nbSynsets;
  map<string, set<string> >& mapping;

};
