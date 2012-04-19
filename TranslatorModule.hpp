#ifndef TRANSLATORMODULE_HPP
#define TRANSLATORMODULE_HPP

#include <string>
#include "../WordNet.hpp"

using namespace std;

class TranslatorModule {
  public : 
  TranslatorModule() ;
  ~TranslatorModule() ;

  virtual void process(WORDNET::WordNet& wn, bool verbose) ;

};


#endif
