#include "Options.hpp"

#include "Paths.hpp"
#include <cassert>

Options::Options(std::string pos, int argc, char **argv) {
  suffix = "";
  noen = false;

  if (pos == "noun") {
    datafile = DATA_NOUN;
  } else if (pos == "verb") {
    datafile = DATA_VERB;
  } else {
    datafile = DATA_ADJ;
  }

  for (int i = 1; i < argc ; i++) {
    std::string param(argv[i]);
    if (param == "EWN") {
      suffix += ".ewn";
      if (pos == "noun") {
        datafile = DATA_NOUN15;
      } else if (pos == "verb") {
        datafile = DATA_VERB15;
      } else if (pos == "adj") {
        datafile = DATA_ADJ15;
      }
    } else if (param == "WOLF") {
      suffix += ".wolf";
      assert(pos == "nouns");
      datafile = DATA_NOUN20;
    /* simple integer: add the corresponding module */
    } else if (argv[i][0] >= '1' && argv[i][0] <= '9') {
      suffix += argv[i][0];
      sequence.push_back(argv[i][0] - '0');
    } else if (param == "Noen") {
      noen = true;
      suffix += "." + param;
    } else {
      suffix += "." + param;
    }
  }
}
