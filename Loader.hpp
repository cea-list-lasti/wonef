#ifndef LOADER_HPP
#define LOADER_HPP

#include <string>
#include "WordNet.hpp"

using namespace std;

class LoaderModule {
  public:
  LoaderModule(string infile, string posfile, string pos) ;
  ~LoaderModule() ;

  static set<string> posList;
  static map<string, string> desaxData;
  static map<string, string> tgt2TgtDefs;
  static WORDNET::WordNetIndex WNIndex;


  WORDNET::WordNet load(bool verbose, int notmore) ;

private:
  map<string, set<string> > src2Tgt;
  string pos;
  bool noen;

  string infile;
  map<string, string> lowercase;

  WORDNET::TgtCandidates extractCandidates(string srcWord);
  bool validPos(string candidatePos);
  void loadBilingualDic();
  void loadLowercase();
  void loadIndex();
  void loadPOSList(string posFile);

  void addInstance(std::map<std::string, std::set<WORDNET::TranslationInfos> >& frenchSynset,
    const std::string& processed, const std::string& translation,
    const std::string& original, int score);
};



#endif
