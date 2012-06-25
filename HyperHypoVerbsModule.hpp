#ifndef HYPERHYPOMODULE_HPP
#define HYPERHYPOMODULE_HPP

#include "TranslatorModule.hpp"
#include "TypeRoler.hpp"


using namespace std;





class HyperHypoVerbsModule : TranslatorModule {
public : 
  HyperHypoVerbsModule(string dataInput, string typeroler, TRMode _mode, int idModuleConf, int nIteration);
  virtual ~HyperHypoVerbsModule();

  virtual void process(WORDNET::WordNet& wn, bool verbose=false) ;
  static string getHead (string locution);
private : 
  TRMode mode;
  TypeRoler tRoler;
  map<string, set<string> >hypernyms;
  map<string, set<string> >hyponyms;
  
  string suffix;

  void loadHyperHypos(string dataInput);

};



#endif
