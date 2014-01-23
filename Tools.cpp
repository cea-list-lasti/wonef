#include "Tools.hpp"
#include "Paths.hpp"

#include <unicode/utypes.h>
#include <unicode/unistr.h>
#include <unicode/translit.h>

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

std::string desaxUTF8(const std::string& str) {
  if (std::all_of(str.cbegin(), str.cend(), [](char c){ return isascii(c); })) {
    return str;
  }

  // UTF-8 std::string -> UTF-16 UnicodeString
  UnicodeString source = UnicodeString::fromUTF8(StringPiece(str));

  // Transliterate UTF-16 UnicodeString
  UErrorCode status = U_ZERO_ERROR;
  Transliterator *accentsConverter = Transliterator::createInstance(
      "NFD; [:M:] Remove; NFC", UTRANS_FORWARD, status);
  accentsConverter->transliterate(source);

  // UTF-16 UnicodeString -> UTF-8 std::string
  std::string result;
  source.toUTF8String(result);

  return result;
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
