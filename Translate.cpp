#include "Dictionaries.hpp"

#include <boost/algorithm/string/join.hpp>
#include <cassert>
#include <iostream>

int main(int argc, char **argv) {

  assert(argc >= 3);
  std::string pos = argv[1];

  Dictionaries dictionaries(pos);
  std::set<std::string> translations;

  bool allFound = true;
  for (int i = 2; i < argc; i++) {
  std::string original = argv[i];
  if (dictionaries.translations.find(original) == dictionaries.translations.end()) {
      std::cerr << original <<  " not found!" << std::endl;
      allFound = false;
    }

    for (const std::string &translation : dictionaries.translations[original]) {
      translations.insert(translation);
    }
  }

  if (allFound) {
    std::cout << boost::algorithm::join(translations, ", ") << std::endl;
  }

  return 0;
}
