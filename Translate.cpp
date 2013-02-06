#include "Dictionaries.hpp"

#include <cassert>
#include <iostream>

int main(int argc, char **argv) {

  assert(argc >= 3);
  std::string pos = argv[1];

  Dictionaries dictionaries(pos);

  for (int i = 2; i < argc; i++) {
    std::string original = argv[i];

    for (const std::string &translation : dictionaries.translations[original]) {
      std::cout << translation << ", ";
    }
  }
  std::cout << std::endl;

  return 0;
}
