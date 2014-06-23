#include <string>
#include "WordNet.hpp"

class DumperModule {
public:
  DumperModule(std::string datafile);

  void dump(WORDNET::WordNet& wn) ;

private:
  std::string datafile;

};
