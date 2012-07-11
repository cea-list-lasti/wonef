#ifndef SIMSYNMODULE_HPP
#define SIMSYNMODULE_HPP


#include "TranslatorModule.hpp"
#include "Loader.hpp"
#include "boost/regex.hpp"
#include "WordNet.hpp"


using namespace std;

class SimSynModule : public TranslatorModule {
public:
  SimSynModule(string _pos, int idModuleConf, int nIteration);
  virtual ~SimSynModule() {}

  virtual void process(WORDNET::WordNet& wn, bool verbose=false) ;

private:
  string knnStdFile;
  pair<string, size_t> selectTgtWord (map<string,int>& candidates,
                  // removes the se_ or s' when pronominal
                  // ex: verbCand["s'étrangler"] = "étrangler"
                  map<string, string>& verbCand,
                  map<string, set<WORDNET::TranslationInfos> >& synset,
                  const string& knnFile);
  string pos;
  string suffix;

protected:
  string trySelectAndReplace(WORDNET::WordNetEntry& synset,
                  pair<string, WORDNET::TgtCandidates> candidate);
};



#endif
