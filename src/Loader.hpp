#ifndef LOADER_HPP
#define LOADER_HPP

#include <string>
#include "Dictionaries.hpp"
#include "WordNet.hpp"



class LoaderModule {
  public:
  LoaderModule(std::string infile, std::string posfile, std::string pos) ;
  ~LoaderModule() ;

  static std::set<std::string> posList;
  static std::map<std::string, std::string> desaxData;
  static WORDNET::WordNetIndex WNIndex;


  WORDNET::WordNet load();

private:
  std::string pos;
  std::string posfile;
  bool noen;

  std::string infile;
  std::map<std::string, std::string> lowercase;
  Dictionaries dictionaries;

  WORDNET::TgtCandidates extractCandidates(std::string srcWord);
  void loadLowercase();
  void loadIndex();
  void loadPOSList(std::string posFile);

  void addInstance(std::map<std::string, std::set<WORDNET::TranslationInfos> >& frenchSynset,
    const std::string& processed, const std::string& translation,
    const std::string& original, int score);
  std::pair<std::string, std::vector<std::string>> readUsages(std::string s);
};



#endif
