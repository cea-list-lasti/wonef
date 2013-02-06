#include "Dictionaries.hpp"

#include <boost/algorithm/string/join.hpp>
#include <cassert>
#include <iostream>

int main(int argc, char **argv) {

  assert(argc >= 3);
  std::string pos = argv[1];

  Dictionaries dictionaries(pos);
  std::set<std::string> translations;

  for (int i = 2; i < argc; i++) {
    std::string original = argv[i];

    for (const std::string &translation : dictionaries.translations[original]) {
      translations.insert(translation);
    }
  }

  std::cout << boost::algorithm::join(translations, ", ") << std::endl;

  return 0;
}
