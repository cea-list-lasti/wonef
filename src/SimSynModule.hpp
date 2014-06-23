#ifndef SIMSYNMODULE_HPP
#define SIMSYNMODULE_HPP


#include "TranslatorModule.hpp"
#include "Loader.hpp"
#include "boost/regex.hpp"
#include "WordNet.hpp"
#include <boost/optional.hpp>



class SimSynModule : public TranslatorModule {
public:
  SimSynModule(std::string _pos, int idModuleConf, int nIteration, bool verbose=false);
  virtual ~SimSynModule() {}

  virtual void process(WORDNET::WordNet& wn) ;

private:
  std::pair<std::string, size_t> selectTgtWord (std::map<std::string,int>& candidates,
                  // removes the se_ or s' when pronominal
                  // ex: verbCand["s'étrangler"] = "étrangler"
                  std::map<std::string, std::string>& verbCand,
                  std::map<std::string, std::set<WORDNET::TranslationInfos> >& synset,
                  const std::string& relation);
  std::string pos;
  std::string suffix;

  std::vector<std::string> rels;

  // map<relation, map<word1, map<word2, distance>>>
  std::map<std::string, std::map<std::string, std::map<std::string, int> > > knnDistsCache;
  boost::optional<int> getDistance(const std::string &relation, const std::string& word, const std::string& related);

  // loads the KnnDists cache from protobuf files
  void loadKnnDistsCache();
  void readProtobuf(std::string relation);

protected:
  std::string trySelectAndReplace(WORDNET::WordNetEntry& synset,
                  std::pair<std::string, WORDNET::TgtCandidates> candidate);
};



#endif
