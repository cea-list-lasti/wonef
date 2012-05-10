#include "SimSynModule.hpp"



using namespace std;

class LastChanceModule : SimSynModule {
public : 
  
  LastChanceModule();
  virtual ~LastChanceModule();

  virtual void process(WORDNET::WordNet& wn, bool verbose=false) ;

  
  string trySelecAndReplace(map<string, set<string> >& synset,
			    string synsetId,
			    map<string, WORDNET::TgtCandidates>::iterator it,
			    bool homograph);
  
};
