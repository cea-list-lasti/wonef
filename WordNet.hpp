#ifndef WORDNET_HPP
#define WORDNET_HPP

#include <string>
#include <vector>
#include <set>
#include <map>

enum class POS { Noun, Verb, Adj, Adv };

namespace WORDNET {

typedef struct TgtCandidates {
  std::map<std::string, int> cand;
  // removes the se_ or s' when pronominal
  // ex: verbCand["s'étrangler"] = "étrangler"
  std::map<std::string, std::string> verbCand;
  std::string formerElected;
  bool capital; // true if the english word is capitalized in WordNet
} TgtCandidates;

struct TranslationInfos {
  std::string original;
  std::string processed;
  float score;
  TranslationInfos() : original(), processed(), score() {}
  TranslationInfos(std::string _original, std::string _processed, float _score) :
    original(_original), processed(_processed), score(_score) { }
  bool operator< (const TranslationInfos& that ) const {
    // sort by score
    return this->score < that.score || this->original < that.original || this->processed < that.processed;
  }
};

typedef struct WordNetEntry {
  //  std::string synsetId;
  POS pos;
  std::string def;
  std::vector<std::string> usages;
  std::string newdef;
  std::set<std::string> hypos;
  std::set<std::string> hypers;
  std::set<std::string> meros;
  std::set<std::string> holos;
  int nbCandidates;
  /* Kept translations */
  std::map<std::string, std::set<TranslationInfos> > frenchSynset;
  /* Candidate translations */
  std::map<std::string, TgtCandidates> frenchCandidates;
  /* Score */
  std::map<std::string, std::set<TranslationInfos> > frenchScore;
} WordNetEntry;

typedef std::map<std::string, WordNetEntry> WordNet;

typedef std::map<std::string, std::set<std::string> > WordNetIndex;

extern std::map<std::string, POS> POS_of_string;
extern std::map<POS, std::string> string_of_POS;
extern std::map<POS, std::string> BalkaNet_string_of_POS;
extern POS pos;

}

#endif
