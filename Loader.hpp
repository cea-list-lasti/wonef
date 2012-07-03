#ifndef LOADER_HPP
#define LOADER_HPP

#include <string>
#include "WordNet.hpp"

using namespace std;

class LoaderModule {
  public : 
  LoaderModule(string infile, set<string>& dicfiles, string posfile, string pos, bool noen) ;
  ~LoaderModule() ;
  
  static set<string> posList;
  static map<string, string> desaxData;
  static map<string, string> tgt2TgtDefs;
  static WORDNET::WordNetIndex WNIndex;
  

  WORDNET::WordNet load(bool verbose, int notmore) ;

private : 
  map<string, set<string> > src2Tgt;
  string pos;
  bool noen;

  set<string> dicfiles;
  string infile;
  map<string, string> lowercase;

  WORDNET::TgtCandidates extractCandidates(string srcWord);
  void loadBilingualDic();
  void loadLowercase();
  void loadIndex();
  //  string tolower(string str);
  void loadPOSList(string posFile);
};



#endif
