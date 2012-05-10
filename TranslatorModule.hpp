#ifndef TRANSLATORMODULE_HPP
#define TRANSLATORMODULE_HPP

#include <string>
#include "WordNet.hpp"

using namespace std;

class TranslatorModule {
  public : 
  TranslatorModule() ;
  virtual ~TranslatorModule() ;

  virtual void process(WORDNET::WordNet& wn, bool verbose) = 0;

};


#endif
