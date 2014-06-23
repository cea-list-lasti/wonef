#ifndef LASTCHANCEMODULE_HPP
#define LASTCHANCEMODULE_HPP


#include "TranslatorModule.hpp"
#include "Loader.hpp"
#include "boost/regex.hpp"
#include "WordNet.hpp"



class LastChanceModule : TranslatorModule {
  public:

  LastChanceModule(int idModuleConf, int nIteration, bool verbose=false);
  virtual ~LastChanceModule();

  virtual void process(WORDNET::WordNet& wn) ;


  std::string trySelecAndReplace(std::map<std::string, std::set<WORDNET::TranslationInfos> >& synset,
			    std::map<std::string, WORDNET::TgtCandidates>::iterator it);

  private :
    std::string suffix;
};

#endif
