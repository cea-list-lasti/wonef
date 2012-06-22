#ifndef SIMSYNMODULE_HPP
#define SIMSYNMODULE_HPP


#include "TranslatorModule.hpp"
#include "Loader.hpp"
#include "boost/regex.hpp"
#include "WordNet.hpp"


using namespace std;

class SimSynModule : public TranslatorModule {
  
public : 
  SimSynModule();
  virtual ~SimSynModule();



  virtual void process(WORDNET::WordNet& wn, bool verbose=false) ;


private : 
  string knnStdFile;
  pair<string, float> selectTgtWord (map<string,int>& candidates,
			map<string, set<WORDNET::TranslationInfos> >& synset,
			string& knnFile);

protected :

  string trySelecAndReplace(map<string, set<WORDNET::TranslationInfos> >& synset,
			  string synsetId,
			  map<string, WORDNET::TgtCandidates >::iterator it,
			  bool homograph=false);
};



#endif
