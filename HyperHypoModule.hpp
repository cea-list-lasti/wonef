#ifndef HYPERHYPOMODULE_HPP
#define HYPERHYPOMODULE_HPP

#include "TranslatorModule.hpp"
#include "TypeRoler.hpp"


using namespace std;





class HyperHypoModule : TranslatorModule {
public : 
  HyperHypoModule(string dataInput, string typeroler);
  ~HyperHypoModule();

  void process(WORDNET::WordNet& wn, TRMode mode, bool verbose=false) ;
  static string getHead (string locution);
private : 
  TypeRoler tRoler;
  map<string, set<string> >hypernyms;
  map<string, set<string> >hyponyms;

  void loadHyperHypos(string dataInput);

};



#endif
