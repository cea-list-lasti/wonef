#ifndef TYPEROLER_HPP
#define TYPEROLER_HPP

#include "tools.h"
#include "Paths.hpp"

#include <set>
#include <utility>
#include <vector>
#include <map>



enum TRMode {R_HYPER, R_HYPO};

class TypeRoler {  

public : 
  TypeRoler(std::string relation) ;
  TypeRoler();
  ~TypeRoler() ;
  float computeIsAScore(std::string strA, std::string strB, TRMode mode);


  std::map<std::string, std::vector<std::pair<ulong,int> > > repository;
  /* multiplications involving two big numbers will overflow normal ints*/
  std::map<std::string, uint> numCoocs;

  std::map<ulong, std::string> dicmap ;
  std::map<std::string, ulong> dicmapReverse ;

private :

  int topSize; 
  float thresCut;
  void writeRepository(std::string protofile, std::string relation);
  void readRepository(std::string protofile, std::string relation);
  int processLine(std::string s, float thresCut, std::vector<std::pair<ulong,int> >& ctxt);
};



#endif
