#ifndef TYPEROLER_HPP
#define TYPEROLER_HPP

#include "../src/knn_search.h"
#include "../src/tools.h"
#include "Paths.hpp"
#include <set>
#include <utility>
#include <vector>
#include <map>

using namespace std;

enum TRMode {R_HYPER, R_HYPO};

class TypeRoler {  
  


public : 
  TypeRoler(string _dataFile, string relation) ;
  TypeRoler();
  ~TypeRoler() ;
  float computeIsAScore( string strA, string strB, TRMode mode);


  map<string, vector<pair<ulong,int> > > repository;
  /* multiplications involving two big numbers will overflow normal ints*/
  map<string, uint> numCoocs;

  map<ulong, string> dicmap ;
  map<string, ulong> dicmapReverse ;

private :

  string dataFile ; 
  int topSize; 
  float thresCut;
  void writeRepository(std::string protofile, std::string relation);
  void readRepository(std::string protofile, std::string relation);
  int processLine(string s, float thresCut, vector<pair<ulong,int> >& ctxt);
};



#endif
