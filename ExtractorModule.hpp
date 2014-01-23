#ifndef EXTRACTOR_MODULE_HPP
#define EXTRACTOR_MODULE_HPP

#include "TranslatorModule.hpp"
class Options;

#include <vector>

enum class ExtractionType {
  Monosemous,
  NoTranslation,
  Uniq,
  MultipleSource,
  Levenshtein
};

class ExtractorModule {
  public:
    ExtractorModule(std::string pos, const Options& options);
    virtual ~ExtractorModule() {}

    virtual void process(WORDNET::WordNet& wn, bool verbose = false);

    static ExtractionType fromInt(int n);
    static std::string toString(ExtractionType e);

  private:
    std::string pos;
    const Options& opt;
    std::map<std::string, std::string> desaxData;

    std::map<std::string, std::set<std::string> > englishCount;

    void addInstance(std::map<std::string, std::set<WORDNET::TranslationInfos> >& frenchSynset,
        const std::string& processed, const std::string& translation,
        const std::string& original, int score);
};

#endif
