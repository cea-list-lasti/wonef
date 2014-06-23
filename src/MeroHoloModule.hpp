#ifndef MEROHOLOMODULE_HPP
#define MEROHOLOMODULE_HPP


#include "TranslatorModule.hpp"
#include "Loader.hpp"
#include "TypeRoler.hpp"
#include "HyperHypoModule.hpp"
#include <boost/regex.hpp>
#include "WordNet.hpp"



enum class MeroHoloMode{MERO, HOLO};

class MeroHoloModule : public TranslatorModule {

public:
  MeroHoloModule(const std::string& datafile, int idModuleConf, int nIteration, bool verbose=false);
  virtual ~MeroHoloModule() {}

  virtual void process(WORDNET::WordNet& wn) ;

protected :
  std::map<ulong, std::string> dicmap ;
  std::map<std::string, std::set<std::string> >reverseIndex;

  std::map<std::string, std::set<std::string> > meronyms;
  std::map<std::string, std::set<std::string> > holonyms;

  TypeRoler meroTypeRoler;
  TypeRoler holoTypeRoler;

  std::string suffix;

  int cntMeros;
  int cntHolos;

  std::string trySelecAndReplace(const WORDNET::WordNet& wn,
          std::string synsetId,
			    WORDNET::WordNetEntry& wne,
			    std::pair<std::string, WORDNET::TgtCandidates>itcand);


  void loadMeroHolos(std::string dataInput);
  float computeIsPartOfScore(const WORDNET::WordNet& wn, const std::string& strA, const std::string& strB);
  float computeIsWholeOfScore(const WORDNET::WordNet& wn, const std::string& strA, const std::string& strB);
  int get_cooc(MeroHoloMode m, const std::string& strA, ulong itlist_fist_code);

};



#endif
