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
  
  TgtCandidates() : cand(), verbCand() {};
  TgtCandidates(const TgtCandidates& c) 
    : cand(c.cand), verbCand(c.verbCand), processed(c.processed), formerElected(c.formerElected)
  {}
} TgtCandidates;

typedef struct WordNetEntry {
  //  std::string synsetId;
  std::string def;
  std::string newdef;
  std::set<std::string> hypos;
  std::set<std::string> hypers;
  std::set<std::string> meros;
  std::set<std::string> holos;
  std::map<std::string, std::set<std::string> >frenchSynset;
  std::map<std::string, TgtCandidates> frenchCandidates;    
} WordNetEntry;

typedef std::map<std::string, WordNetEntry> WordNet;


typedef std::map<std::string, std::set<std::string> > WordNetIndex;

}


#endif
