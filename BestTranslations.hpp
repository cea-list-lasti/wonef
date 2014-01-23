#ifndef BESTTRANSLATIONS_HPP
#define BESTTRANSLATIONS_HPP

#include "WordNet.hpp"
class Options;

class BestTranslations {
  public:
    BestTranslations(const Options& options, bool easyFilter = false);

    void choose(WORDNET::WordNet& wn);
  private:
    const Options& options;
    bool easyFilter;
};

#endif
