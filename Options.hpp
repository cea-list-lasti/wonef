#ifndef _OPTIONS_H__
#define _OPTIONS_H__

#include <string>
#include <vector>

/* A simple utility class that parses options */
class Options {
  public:
    Options(std::string pos, int argc, char **argv);

    std::string suffix;
    std::vector<int> sequence;
    std::string datafile;
    bool noen;

};

#endif
