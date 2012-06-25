#ifndef LASTCHANCEMODULE_HPP
#define LASTCHANCEMODULE_HPP


#include "TranslatorModule.hpp"
#include "Loader.hpp"
#include "boost/regex.hpp"
#include "WordNet.hpp"



using namespace std;

class LastChanceModule : TranslatorModule {
  public : 
  
  LastChanceModule(int idModuleConf, int nIteration);
  virtual ~LastChanceModule();

  virtual void process(WORDNET::WordNet& wn, bool verbose=false) ;

  
  string trySelecAndReplace(map<string, set<WORDNET::TranslationInfos> >& synset,
			    string synsetId,
			    map<string, WORDNET::TgtCandidates>::iterator it,
			    bool homograph);

  private :
    string suffix;
};

#endif