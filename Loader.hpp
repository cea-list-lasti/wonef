#ifndef LOADER_HPP
#define LOADER_HPP

#include <string>
#include "Dictionaries.hpp"
#include "WordNet.hpp"

using namespace std;

class LoaderModule {
  public:
  LoaderModule(string infile, string posfile, string pos) ;
  ~LoaderModule() ;

  static set<string> posList;
  static map<string, string> desaxData;
  static WORDNET::WordNetIndex WNIndex;


  WORDNET::WordNet load();

private:
  string pos;
  bool noen;

  string infile;
  map<string, string> lowercase;
  Dictionaries dictionaries;

  WORDNET::TgtCandidates extractCandidates(string srcWord);
  void loadLowercase();
  void loadIndex();
  void loadPOSList(string posFile);

  void addInstance(std::map<std::string, std::set<WORDNET::TranslationInfos> >& frenchSynset,
    const std::string& processed, const std::string& translation,
    const std::string& original, int score);
  std::pair<std::string, std::vector<std::string>> readUsages(std::string s);
};



#endif
