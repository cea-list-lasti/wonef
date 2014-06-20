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

Options::Options(std::string _pos, int argc, char **argv) {
  OptionMode omode(OptionMode::Module);
  std::string extractionSuffix = ".e";
  std::string moduleSuffix = ".m";

  POS pos = WORDNET::POS_of_string[_pos];

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
    switch(pos) {
      case POS::Noun:
        switch(mode) {
          case Mode::Precision:
            extractions = {ExtractionType::Monosemous, ExtractionType::Uniq, ExtractionType::Levenshtein};
            break;
          case Mode::FScore:
          case Mode::Coverage:
            extractions = {ExtractionType::Monosemous, ExtractionType::Uniq, ExtractionType::MultipleSource, ExtractionType::Levenshtein};
            break;
          default: assert(false);
        }
        break;
      case POS::Verb:
        switch(mode) {
          case Mode::Precision:
            extractions = {ExtractionType::Uniq};
            break;
          case Mode::FScore:
            extractions = {ExtractionType::Monosemous, ExtractionType::Uniq};
            break;
          case Mode::Coverage:
            extractions = {ExtractionType::Monosemous, ExtractionType::Uniq, ExtractionType::MultipleSource};
            break;
          default: assert(false);
        }
        break;
      case POS::Adj:
        switch(mode) {
          case Mode::Precision:
            extractions = {ExtractionType::Monosemous, ExtractionType::Uniq, ExtractionType::Levenshtein};
            break;
          case Mode::FScore:
          case Mode::Coverage:
            extractions = {ExtractionType::Monosemous, ExtractionType::Uniq, ExtractionType::MultipleSource, ExtractionType::Levenshtein};
            break;
          default: assert(false);
        }
        break;
      case POS::Adv:
        extractions = {ExtractionType::Monosemous, ExtractionType::Uniq, ExtractionType::MultipleSource, ExtractionType::Levenshtein};
        break;
      default: assert(false);
    }
  }

  std::cout << "extractions: ";
  for(ExtractionType e: extractions) {
    std::cout << ExtractorModule::toString(e) << " ";
  }
  std::cout << std::endl;

  std::cout << "modules: ";
  for(int i: moduleSequence) {
    std::cout << i << " ";
  }
  std::cout << std::endl;
}

std::string Options::getWN30Data(POS pos) {
  switch(pos) {
    case POS::Noun: return DATA_NOUN30;
    case POS::Verb: return DATA_VERB30;
    case POS::Adj:  return DATA_ADJ30;
    case POS::Adv:  return DATA_ADV30;
    default: assert(false);
  }
}
