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

public:
  MeroHoloModule(string& datafile, int idModuleConf, int nIteration, bool verbose=false);
  virtual ~MeroHoloModule();


  void finalize();
  virtual void process(WORDNET::WordNet& wn) ;

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

  string trySelecAndReplace(const WORDNET::WordNet& wn,
          std::string synsetId,
			    WORDNET::WordNetEntry& wne,
			    pair<string, WORDNET::TgtCandidates>itcand);


  void loadMeroHolos(string dataInput);
  float computeIsPartOfScore(const WORDNET::WordNet& wn, const string& strA, const string& strB);
  float computeIsWholeOfScore(const WORDNET::WordNet& wn, const string& strA, const string& strB);
  void processLine(ulong currentId, string s, Mode mode);

};



#endif
