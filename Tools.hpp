#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <string>
#include <map>

std::string tolower(std::string strToConvert);
std::string desaxUTF8(const std::string& str);

std::map<std::string, std::string> loadSensemap(std::string pos);

#endif
