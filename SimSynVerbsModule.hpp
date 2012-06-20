#ifndef SIMSYNVERBSMODULE_HPP
#define SIMSYNVERBSMODULE_HPP


#include "TranslatorModule.hpp"
#include "LoaderVerbs.hpp"
#include "boost/regex.hpp"
#include "WordNet.hpp"


using namespace std;

class SimSynVerbsModule : public TranslatorModule {
  
public : 
  SimSynVerbsModule();
  virtual ~SimSynVerbsModule();



  virtual void process(WORDNET::WordNet& wn, bool verbose=false) ;


private : 
  string knnStdFile;
  pair<string, float> selectTgtWord (map<string,int>& candidates,
			// removes the se_ or s' when pronominal
			// ex: verbCand["s'étrangler"] = "étrangler"
			map<string, string>& verbCand,
			map<string, set<pair<string, float> > >& synset,
			string& knnFile);

protected :

  string trySelecAndReplace(map<string, set<pair<string, float> > >& synset,
			  string synsetId,
			  map<string, WORDNET::TgtCandidates >::iterator it,
			  bool homograph=false);
};



#endif
