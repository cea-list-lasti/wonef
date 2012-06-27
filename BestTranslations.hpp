#ifndef BESTTRANSLATIONS_HPP
#define BESTTRANSLATIONS_HPP

#include "WordNet.hpp"

class BestTranslations {
  public :
    BestTranslations();
    ~BestTranslations();

    void choose(WORDNET::WordNet& wn);
};

#endif