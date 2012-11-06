#include <string>
#include "WordNet.hpp"

using namespace std;

class DumperModule {
public:
  DumperModule(string datafile);

  void dump(WORDNET::WordNet& wn) ;

private:
  string datafile;

};
