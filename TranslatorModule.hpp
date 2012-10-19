#ifndef TRANSLATORMODULE_HPP
#define TRANSLATORMODULE_HPP

#include "WordNet.hpp"

class TranslatorModule {
  public:
    TranslatorModule(bool verbose = false) ;
    virtual ~TranslatorModule() ;

    virtual void process(WORDNET::WordNet& wn) = 0;
  protected:
    bool verbose;
};

#endif
