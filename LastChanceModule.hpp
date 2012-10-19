#ifndef LASTCHANCEMODULE_HPP
#define LASTCHANCEMODULE_HPP


#include "TranslatorModule.hpp"
#include "Loader.hpp"
#include "boost/regex.hpp"
#include "WordNet.hpp"



using namespace std;

class LastChanceModule : TranslatorModule {
  public:

  LastChanceModule(int idModuleConf, int nIteration, bool verbose=false);
  virtual ~LastChanceModule();

  virtual void process(WORDNET::WordNet& wn) ;


  string trySelecAndReplace(map<string, set<WORDNET::TranslationInfos> >& synset,
			    map<string, WORDNET::TgtCandidates>::iterator it);

  private :
    string suffix;
};

#endif
