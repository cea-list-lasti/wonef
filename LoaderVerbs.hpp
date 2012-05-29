#ifndef LOADER_VERBS_HPP
#define LOADER_VERBS_HPP

#include <string>
#include "WordNet.hpp"

using namespace std;

class LoaderVerbsModule {
  public : 
  LoaderVerbsModule(string infile, set<string>& dicfiles, string verbfile) ;
  ~LoaderVerbsModule() ;
  
  static set<string> verbsList;
  static map<string, string> desaxData;
  static map<string, string> tgt2TgtDefs;
  static WORDNET::WordNetIndex WNIndex;
  

  WORDNET::WordNet load(bool verbose, int notmore) ;

private : 
  map<string, set<string> > src2Tgt;

  set<string> dicfiles;
  string infile;
  map<string, string> lowercase;

  WORDNET::TgtCandidates extractCandidates(string srcWord);
  void loadBilingualDic();
  void loadLowercase();
  void loadIndex();
  //  string tolower(string str);
  void loadPOSList(string verbsFile);
};



#endif
