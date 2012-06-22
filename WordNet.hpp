#ifndef WORDNET_HPP
#define WORDNET_HPP

#include <string>
#include <vector>
#include <set>
#include <map>

namespace WORDNET {


typedef struct TgtCandidates {
  std::map<std::string, int> cand;
  // removes the se_ or s' when pronominal
  // ex: verbCand["s'étrangler"] = "étrangler"
  std::map<std::string, std::string> verbCand;
  std::string processed;
  std::string formerElected;
} TgtCandidates;

struct TranslationInfos {
  std::string original;
  std::string processed;
  float score;
  TranslationInfos() : original(), processed(), score() {}
  bool operator< (const TranslationInfos& that ) const {
    return this->original < that.original || this->processed < that.processed;
  }
};

typedef struct WordNetEntry {
  //  std::string synsetId;
  std::string def;
  std::string newdef;
  std::set<std::string> hypos;
  std::set<std::string> hypers;
  std::set<std::string> meros;
  std::set<std::string> holos;
  std::map<std::string, std::set<TranslationInfos> > frenchSynset;
  std::map<std::string, TgtCandidates> frenchCandidates;
} WordNetEntry;

typedef std::map<std::string, WordNetEntry> WordNet;

typedef std::map<std::string, std::set<std::string> > WordNetIndex;


}

#endif
