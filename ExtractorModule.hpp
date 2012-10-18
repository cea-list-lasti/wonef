#ifndef EXTRACTOR_MODULE_HPP
#define EXTRACTOR_MODULE_HPP

#include <vector>
#include "TranslatorModule.hpp"

enum class ExtractionType {
  Monosemous,
  NoTranslation,
  Uniq,
  MultipleSource
};

class ExtractorModule {
  public:
    ExtractorModule(std::string pos, std::set<ExtractionType>);
    virtual ~ExtractorModule() {}

    virtual void process(WORDNET::WordNet& wn, bool verbose = false);

    static ExtractionType fromInt(int n);
    static std::string toString(ExtractionType e);

  private:
    std::string pos;
    std::set<ExtractionType> extractions;
    std::map<std::string, std::string> desaxData;

    std::map<std::string, std::set<std::string> > englishCount;

    void addInstance(std::map<std::string, std::set<WORDNET::TranslationInfos> >& frenchSynset,
        const std::string& processed, const std::string& translation,
        const std::string& original, int score);
};

#endif
