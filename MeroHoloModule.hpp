#ifndef MEROHOLOMODULE_HPP
#define MEROHOLOMODULE_HPP


#include "TranslatorModule.hpp"
#include "Loader.hpp"
#include "TypeRoler.hpp"
#include "HyperHypoModule.hpp"
#include <boost/regex.hpp>
#include "WordNet.hpp"


using namespace std;

enum class MeroHoloMode{MERO, HOLO};


class MeroHoloModule : public TranslatorModule {

public:
  MeroHoloModule(string& datafile, int idModuleConf, int nIteration, bool verbose=false);
  virtual ~MeroHoloModule() {}

  virtual void process(WORDNET::WordNet& wn) ;

protected :
  map<ulong, string> dicmap ;
  map<string, set<string> >reverseIndex;

  map<string, set<string> > meronyms;
  map<string, set<string> > holonyms;

  TypeRoler meroTypeRoler;
  TypeRoler holoTypeRoler;

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
  int get_cooc(MeroHoloMode m, const string& strA, ulong itlist_fist_code);

};



#endif
