#ifndef DICTIONARIES_HPP
#define DICTIONARIES_HPP

#include <map>
#include <set>
#include <string>

class Dictionaries {
  public:
    Dictionaries(std::string pos);
    static std::map<std::string, std::string> definition;
    std::map<std::string, std::set<std::string>> translations;

  private:
    bool validPos(std::string wantedPos, std::string candidatePos);


};

#endif
