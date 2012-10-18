#include "Options.hpp"

#include "Paths.hpp"

#include <cassert>
#include <iostream>

enum class OptionMode { Module, Extraction };

Options::Options(std::string pos, int argc, char **argv) {
  OptionMode mode(OptionMode::Module);
  std::string extractionSuffix = ".e";
  std::string moduleSuffix = ".m";

  datafile = getWN30Data(pos);

  for (int i = 1; i < argc ; i++) {
    std::string param(argv[i]);
    if (param == "--module") {
      mode = OptionMode::Module;
    } else if (param == "--extract") {
      mode = OptionMode::Extraction;
    } else if (param[0] >= '1' && param[0] <= '9') {
      /* simple integer: add the corresponding module */
      int n = param[0] - '0';
      if (mode == OptionMode::Module) {
        moduleSuffix += param[0];
        moduleSequence.push_back(n);
      } else if (mode == OptionMode::Extraction) {
        extractionSuffix += param[0];
        extractionSet.insert(ExtractorModule::fromInt(n));
      }
    } else {
      std::cerr << "Unexpected param: " << param;
      exit(-1);
    }
  }

  suffix = extractionSuffix + moduleSuffix;

  if (extractionSet.empty()) {
    extractionSet = {ExtractionType::Monosemous, ExtractionType::NoTranslation, ExtractionType::Uniq};
  }

  std::cout << "extractions: ";
  for(ExtractionType e: extractionSet) {
    std::cout << ExtractorModule::toString(e) << " ";
  }
  std::cout << std::endl;
}

std::string Options::getWN30Data(std::string pos) {
  if (pos == "noun") {
    return DATA_NOUN30;
  } else if (pos == "verb") {
    return DATA_VERB30;
  } else if (pos =="adj") {
    return DATA_ADJ30;
  } else {
    assert(false);
  }
}
