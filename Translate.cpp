#include "Dictionaries.hpp"

#include <cassert>
#include <iostream>

int main(int argc, char **argv) {

  assert(argc >= 3);
  std::string pos = argv[1];

  Dictionaries dictionaries(pos);
  std::map<std::string, int> translations;

  for (int i = 2; i < argc; i++) {
      std::string original = argv[i];
      if (dictionaries.translations.find(original) == dictionaries.translations.end()) {
          std::cerr << original <<  " not found!" << std::endl;
      }

      for (const std::string &translation : dictionaries.translations[original]) {
          translations[translation]++;
      }
  }

  for(auto& translation: translations) {
      std::cout << translation.second << " " << translation.first<< std::endl;
  }

  return 0;
}
