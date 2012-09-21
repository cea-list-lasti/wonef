#ifndef TRANSLATORMODULE_HPP
#define TRANSLATORMODULE_HPP

#include "WordNet.hpp"

class TranslatorModule {
  public:
    TranslatorModule() ;
    virtual ~TranslatorModule() ;

    virtual void process(WORDNET::WordNet& wn, bool verbose) = 0;
};

#endif
