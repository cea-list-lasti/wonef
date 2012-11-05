#ifndef BESTTRANSLATIONS_HPP
#define BESTTRANSLATIONS_HPP

#include "WordNet.hpp"

class BestTranslations {
  public:
    BestTranslations(bool easyFilter = false);

    void choose(WORDNET::WordNet& wn);
  private:
    bool easyFilter;
};

#endif
