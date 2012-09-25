#ifndef _OPTIONS_H__
#define _OPTIONS_H__

#include "ExtractorModule.hpp"

#include <string>
#include <vector>

/* A simple utility class that parses options */
class Options {
  public:
    Options(std::string pos, int argc, char **argv);

    std::string suffix;
    std::vector<int> moduleSequence;
    std::set<ExtractionType> extractionSet;
    std::string datafile;
    bool noen;

  private:
    std::string getWN15Data(std::string pos);
    std::string getWN20Data(std::string pos);

};

#endif
