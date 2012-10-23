#include "Tools.hpp"

#include "Paths.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

string tolower(string strToConvert)
{//change each element of the string to lower case
   for(unsigned int i=0;i<strToConvert.length();i++)
   {
      strToConvert[i] = tolower(strToConvert[i]);
   }
   return strToConvert;//return the converted string
}

std::map<std::string, std::string> loadSensemap(std::string pos) {
  std::map<std::string, std::string> sensemap;
  std::ifstream infile(MAPPING_20_30 + pos);
  std::string line, source, target;

  if (infile.fail()) {
    std::cerr << "Oops, " << MAPPING_20_30 + pos << " doesn't exist. " << __FILE__ << ":" << __LINE__ << endl;
    exit(-1);
  }

  /* We're ignoring the 47 mappings (0.05%) with more than one target synset */
  while (std::getline(infile, line)) {
    std::istringstream iss(line);
    iss >> source >> target;
    sensemap[source] = target;
  }

  return sensemap;
}
