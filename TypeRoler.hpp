#ifndef TYPEROLER_HPP
#define TYPEROLER_HPP

#include "../src/knn_search.h"
#include "../src/converter.h"
#include "../src/tools.h"
#include "Paths.hpp"
#include <set> 

using namespace std;

enum TRMode {R_HYPER, R_HYPO};

class TypeRoler {  
  


public : 
  TypeRoler(string _dataFile, string relation) ;
  TypeRoler();
  ~TypeRoler() ;
  float computeIsAScore( string strA, string strB, TRMode mode);
  



private :
  map<ulong, string> dicmap ;
  map<string, ulong> dicmapReverse ;

  string dataFile ; 
  Converter conv;
  int topSize; 
  float thresCut;
  map<string, vector<ulong> > repository;
  void processLine(string s, float thresCut, vector<ulong>& ctxt);
};



#endif
