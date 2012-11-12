#include "Options.hpp"

#include "Paths.hpp"

#include <cassert>
#include <iostream>

enum class OptionMode { Module, Extraction };

Mode Mode_of_string(std::string s) {
  if(s == "fscore") { return Mode::FScore; }
  else if (s == "precision") { return Mode::Precision; }
  else if (s == "coverage") { return Mode::Coverage; }
  else {
    std::cerr << "unknown mode " << s << std::endl;
    exit(-1);
  }
}

Options::Options(std::string pos, int argc, char **argv) {
  OptionMode omode(OptionMode::Module);
  std::string extractionSuffix = ".e";
  std::string moduleSuffix = ".m";

  datafile = getWN30Data(pos);

  mode = Mode::FScore;

  for (int i = 1; i < argc ; i++) {
    std::string param(argv[i]);
    if (param == "--module") {
      omode = OptionMode::Module;
    } else if (param == "--extract") {
      omode = OptionMode::Extraction;
    } else if (param[0] >= '1' && param[0] <= '9') {
      /* simple integer: add the corresponding module */
      int n = param[0] - '0';
      if (omode == OptionMode::Module) {
        moduleSuffix += param[0];
        moduleSequence.push_back(n);
      } else if (omode == OptionMode::Extraction) {
        extractionSuffix += param[0];
        extractions.insert(ExtractorModule::fromInt(n));
      }
    } else if (param == "--mode") {
      if (i+1 >= argc) {
        std::cerr << "Expecting a mode after --mode" << std::endl;
        exit(-1);
      } else {
        std::string modeChoice(argv[i+1]);
        mode = Mode_of_string(modeChoice);
        moduleSuffix += "." + modeChoice;
        i++;
      }
    } else {
      std::cerr << "Unexpected param: " << param;
      exit(-1);
    }
  }

  suffix = extractionSuffix + moduleSuffix;

  if (extractions.empty()) {
    extractions = {ExtractionType::Monosemous, ExtractionType::NoTranslation, ExtractionType::Uniq};
  }

  std::cout << "extractions: ";
  for(ExtractionType e: extractions) {
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
