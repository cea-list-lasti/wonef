#include "Options.hpp"

#include "Paths.hpp"
#include <cassert>
#include <iostream>

enum class OptionMode { Module, Extraction };

Options::Options(std::string pos, int argc, char **argv) {
  OptionMode mode(OptionMode::Module);
  suffix = "";

  datafile = getWN20Data(pos);

  for (int i = 1; i < argc ; i++) {
    std::string param(argv[i]);
    if (param == "--module") {
      mode = OptionMode::Module;
      suffix += ".m";
    } else if (param == "--extract") {
      mode = OptionMode::Extraction;
      suffix += ".e";
    } else if (param[0] >= '1' && param[0] <= '9') {
      /* simple integer: add the corresponding module */
      int n = param[0] - '0';
      suffix += param[0];
      if (mode == OptionMode::Module) {
        std::cout << "module " << n << std::endl;
        moduleSequence.push_back(n);
      } else if (mode == OptionMode::Extraction) {
        std::cout << "extracting " << n << std::endl;
        extractionSet.insert(ExtractorModule::fromInt(n));
      }
    } else {
      std::cerr << "Unexpected param: " << param;
      exit(-1);
    }
  }

  if (extractionSet.empty()) {
    extractionSet = {ExtractionType::Monosemous, ExtractionType::NoTranslation, ExtractionType::Uniq};
  } else {
    std::cout << "non empty" << std::endl;
  }
}

std::string Options::getWN20Data(std::string pos) {
  if (pos == "noun") {
    return DATA_NOUN;
  } else if (pos == "verb") {
    return DATA_VERB;
  } else if (pos =="adj") {
    return DATA_ADJ;
  } else {
    assert(false);
  }
}
