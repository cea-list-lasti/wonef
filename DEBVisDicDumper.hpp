#include "WordNet.hpp"

class DEBVisDicDumperModule {
public:
  DEBVisDicDumperModule(std::string _datafile) : datafile(_datafile) {}
  void dump(WORDNET::WordNet& wn);

private:
  std::string datafile;
};
