#ifndef SIMSYNMODULE_HPP
#define SIMSYNMODULE_HPP


#include "TranslatorModule.hpp"
#include "Loader.hpp"
#include "boost/regex.hpp"
#include "WordNet.hpp"
#include <boost/optional.hpp>


using namespace std;

class SimSynModule : public TranslatorModule {
public:
  SimSynModule(string _pos, int idModuleConf, int nIteration, bool verbose=false);
  virtual ~SimSynModule() {}

  virtual void process(WORDNET::WordNet& wn) ;

private:
  pair<string, size_t> selectTgtWord (map<string,int>& candidates,
                  // removes the se_ or s' when pronominal
                  // ex: verbCand["s'étrangler"] = "étrangler"
                  map<string, string>& verbCand,
                  map<string, set<WORDNET::TranslationInfos> >& synset,
                  const string& relation);
  string pos;
  string suffix;

  std::vector<string> rels;

  // map<relation, map<word1, map<word2, distance>>>
  std::map<std::string, std::map<std::string, std::map<std::string, int> > > knnDistsCache;
  boost::optional<int> getDistance(const string &relation, const string& word, const string& related);

  // load write the cache if necessary, or reads it otherwise.
  void loadKnnDistsCache();
  void buildRelationCache(std::string relation);

  // read and write protocol buffers
  void readProtobuf(std::string relation);
  void writeProtobuf(std::string relation);

  std::list<std::string> getPaths(std::string relation);

protected:
  string trySelectAndReplace(WORDNET::WordNetEntry& synset,
                  pair<string, WORDNET::TgtCandidates> candidate);
};



#endif
