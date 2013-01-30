#include "Dictionaries.hpp"

#include <cassert>
#include <iostream>

int main(int argc, char **argv) {

  assert(argc == 3);
  std::string pos = argv[1];
  std::string original = argv[2];

  Dictionaries dictionaries(pos);

  for (const std::string &translation : dictionaries.translations[original]) {
    std::cout << translation << std::endl;
  }

  return 0;
}
