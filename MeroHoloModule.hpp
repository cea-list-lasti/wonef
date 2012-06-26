#ifndef MEROHOLOMODULE_HPP
#define MEROHOLOMODULE_HPP


#include "../src/knn_search.h"
#include "TranslatorModule.hpp"
#include "Loader.hpp"
#include "TypeRoler.hpp"
#include "HyperHypoModule.hpp"
#include <boost/regex.hpp>
#include "WordNet.hpp"


using namespace std;

enum Mode{MERO, HOLO};


class MeroHoloModule : public TranslatorModule {
  
public : 
  MeroHoloModule();
  MeroHoloModule(string& datafile, int idModuleConf, int nIteration);
  virtual ~MeroHoloModule();


  void finalize();
  virtual void process(WORDNET::WordNet& wn, bool verbose=false) ;
 
protected :

  map<ulong, string> dicmap ;
  map<string, set<string> >reverseIndex;

  map<string, set<string> > meronyms;
  map<string, set<string> > holonyms;

  map<string, map<string, int> > coocsMero;
  map<string, map<string, int> > coocsHolo;

  std::map<string, uint> sumMeros;
  std::map<string, uint> sumHolos;

  string suffix;

  int cntMeros; 
  int cntHolos;

  string trySelecAndReplace(WORDNET::WordNet& wn,
			    map<string, WORDNET::WordNetEntry>::iterator itwne,
			    map<string, WORDNET::TgtCandidates >::iterator itcand,
			    bool verbose);


  void loadMeroHolos(string dataInput);
  float computeIsPartOfScore(WORDNET::WordNet& wn, string strA, string strB, bool verbose);
  float computeIsWholeOfScore(WORDNET::WordNet& wn, string strA, string strB, bool verbose);
  void processLine(ulong currentId, string s, Mode mode);

};



#endif
