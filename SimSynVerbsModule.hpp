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
  string selectTgtWord (map<string,int>& candidates,
			map<string, set<string> >& synset,
			string& knnFile);

protected :

  string trySelecAndReplace(map<string, set<string> >& synset,
			  string synsetId,
			  map<string, WORDNET::TgtCandidates >::iterator it,
			  bool homograph=false);
};



#endif
