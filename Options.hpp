#ifndef _OPTIONS_H__
#define _OPTIONS_H__

#include "ExtractorModule.hpp"
#include "WordNet.hpp"

#include <string>
#include <vector>

enum class Mode {
  FScore,
  Precision,
  Coverage
};

/* A simple utility class that parses options */
class Options {
  public:
    Options(std::string pos, int argc, char **argv);

    std::string suffix;
    std::vector<int> moduleSequence;
    std::set<ExtractionType> extractions;
    std::string datafile;
    bool noen;
    Mode mode;

  private:
    std::string getWN30Data(POS pos);

};

#endif
