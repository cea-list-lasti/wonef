#include "SimSynModule.hpp"
#include "knn.pb.h"
#include "distance.hpp"
#include "Paths.hpp"
#include "../src/tools.h"
#include <iostream>
#include <fstream>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex/icu.hpp>
#include <list>

using namespace std;

SimSynModule::SimSynModule(string _pos, int idModuleConf, int nIteration) : pos(_pos) {
  std::ostringstream oss;
  oss << idModuleConf << "." << nIteration;
  suffix = oss.str();

  if (pos == "noun") {
    rels.push_back("COMPDUNOM");
    rels.push_back("COD_V");
    rels.push_back("APPOS");
  } else if (pos == "verb") {
    rels.push_back("COD_V.reverse");
    rels.push_back("CPL_V.reverse");
    rels.push_back("CPLV_V.reverse");
    //rels.push_back("SUJ_V.reverse");
  } else if (pos == "adj") {
    rels.push_back("SUBADJPOST");
    rels.push_back("ADVADJ.reverse");
    rels.push_back("ATB_SG");
  }

  loadKnnDistsCache();

}

void SimSynModule::process(WORDNET::WordNet& wn, bool /*verbose*/){
  typedef std::pair<const string, WORDNET::WordNetEntry> synset_t;
  typedef std::pair<const string, WORDNET::TgtCandidates> candidate_t;

  BOOST_FOREACH(synset_t& synset, wn) {
    /* For every potential translation in our synset, look for already
     * translated synonyms */
    BOOST_FOREACH(candidate_t& candidate, synset.second.frenchCandidates) {
      if (candidate.second.cand.size() > 0) {
        synset.second.newdef = trySelectAndReplace(synset.second, candidate);
      }
    }
  }
}

string SimSynModule::trySelectAndReplace(WORDNET::WordNetEntry& synset,
  pair<string, WORDNET::TgtCandidates> candidate) {

  unsigned int i = 0;
  size_t maximumValue = 0;
  // TODO need to learn again when more data available and the algorithm is fixed.
  if (pos == "verb") {
    maximumValue = 6006;
  } else if (pos == "noun") {
    maximumValue = 7273;
  }

  set<pair<string, size_t> > elected;

  while ((elected.empty() || elected.begin()->second >= maximumValue) && i < rels.size()) {
    pair<string, size_t> elec = selectTgtWord(candidate.second.cand, candidate.second.verbCand, synset.frenchSynset, rels[i]);
    if (elec.first != "") {
      // First time, "elected" is empty.
      // Then, we keep only the "elec" with shortest distance.
      if (elected.empty() || (!elected.empty() && elec.second < elected.begin()->second)) {
        elected.clear();
        elected.insert(elec);
      }
    }
    i++;
  }

  if (!elected.empty()) {
    for (set<pair<string, size_t> >::iterator itElec = elected.begin(); itElec != elected.end(); itElec++) {
      WORDNET::TranslationInfos translationInfos;
      translationInfos.original = candidate.first;
      translationInfos.processed = "simsyn" + suffix;
      translationInfos.score = itElec->second;
      if (translationInfos.score < maximumValue) {
        synset.frenchSynset[itElec->first].insert(translationInfos);
      }
      synset.frenchScore[itElec->first].insert(translationInfos);
    }
    return LoaderModule::tgt2TgtDefs[(*elected.begin()).first];
  }
  return "";
}

boost::optional<int> SimSynModule::getDistance(const string &relation, const string& word, const string& related) {
  std::map<std::string, std::map<std::string, int> >& relationCache = knnDistsCache[relation];

  if (relationCache.find(word) == relationCache.end()
      || relationCache[word].find(related) == relationCache[word].end()) {
    return boost::optional<int>();
  } else {
    // restore the value from the cache
    return boost::optional<int>(relationCache[word][related]);
  }
}

std::list<std::string> SimSynModule::getPaths(std::string relation) {
  std::list<std::string> paths;
  boost::filesystem::path dir(KNNSTDDIR + relation);
  assert(boost::filesystem::is_directory(dir));

  boost::filesystem::directory_iterator it(dir);
  boost::filesystem::directory_iterator end;

  while (it != end) {
    if (it->path().extension() == ".txt") {
      paths.push_back(it->path().native());
    }
    it++;
  }

  return paths;
}

void SimSynModule::buildRelationCache(std::string relation) {
  std::map<std::string, std::map<std::string, int> >& relationCache = knnDistsCache[relation];

  BOOST_FOREACH(const std::string& knnFile, getPaths(relation)) {
    // put our file into a string
    string knns;
    ifstream knnIfs(knnFile);
    if (knnIfs.fail()) {
      std::cerr << "Oops, " << knnFile << " doesn't exist. " << __FILE__ << ":" << __LINE__ << std::endl;
      exit(-1);
    }
    getline(knnIfs, knns);
    knnIfs.close();

    boost::u32regex r = boost::make_u32regex("([a-z]+):([0-9]+)", boost::regex_constants::collate);

    boost::u32regex_token_iterator<std::string::const_iterator>
      iterword(knns.begin(), knns.end(), r, 1),
      endword;
    boost::u32regex_token_iterator<std::string::const_iterator>
      iterscore(knns.begin(), knns.end(), r, 2),
      endscore;


    std::map<std::string, int>& relatedWords = relationCache[*iterword];

    // skip first word
    iterword++;
    iterscore++;

    for( ; iterword != endword; ++iterword, ++iterscore ) {
        int score;
        stringstream(*iterscore) >> score;
        relatedWords[*iterword] = score;
    }

  }

}

pair<string, size_t> SimSynModule::selectTgtWord (map<string, int>& cand, map<string, string>& verbCand, map<string, set<WORDNET::TranslationInfos> >& synset, const string& relation) {
  map<pair<string, size_t>, uint> votes;
  for (map<string, set<WORDNET::TranslationInfos> >::iterator itSynset = synset.begin(); itSynset != synset.end(); itSynset++) {

    int bestDist = INT_MAX;
    pair<string, size_t> syn;
    syn.first = "";
    for (map<string,int>::iterator it2 = cand.begin(); it2!=cand.end(); it2++) {
      /* Look at the position of our candidate in the synonyms list */
      std::string possiblyRelated = pos == "verb" ? verbCand[it2->first] : it2->first;

      boost::optional<int> maybe_semantic_distance = getDistance(relation, itSynset->first, possiblyRelated);
      if (maybe_semantic_distance) {
        int semantic_distance = *maybe_semantic_distance;
        pair<string, float> candidate;
        candidate.first = it2->first;
        candidate.second = semantic_distance;
        votes[candidate] += it2->second;
        if(semantic_distance < bestDist && semantic_distance > 0) {
          bestDist = semantic_distance;
          syn = candidate;
        }
      }
    }
    // TODO:  ponderate score with proximity
    votes[syn]++;
  }

  size_t bestVote = 0;
  pair<string, size_t> elected;
  for(map<pair<string, size_t>, uint>::iterator itVotes = votes.begin(); itVotes != votes.end(); itVotes++)  {
    if (itVotes->second > bestVote && (itVotes->first).first != "") {
      bestVote = itVotes->second;
      elected = itVotes->first;
    }
  }
  return elected;
}



// precondition: protofile contains the correct data
void SimSynModule::readProtobuf(std::string relation) {
  std::string protofile = KNNPROTOFILE + pos + "_" + relation;

  KnnDists knnDistsFile;
  fstream in(protofile.c_str(), ios::in | ios::binary);
  if (!knnDistsFile.ParseFromIstream(&in)) {
    std::cerr << "Failed to parse " << protofile << std::endl;
    exit(1);
  }

  std::map<std::string, std::map<std::string, int> >& relationCache = knnDistsCache[relation];

  assert(relation == knnDistsFile.relation());
  for (int i = 0; i < knnDistsFile.word_size(); i++) {
    KnnDists::Word w = knnDistsFile.word(i);

    assert(w.related_size() == w.distance_size());
    for (int j = 0; j < w.related_size(); j++) {
      relationCache[w.word()][w.related(j)] = w.distance(j);
    }
  }
}

// postcondition: protofile contains the correct data
void SimSynModule::writeProtobuf(std::string relation) {
  std::string protofile = KNNPROTOFILE + pos + "_" + relation;
  KnnDists knnDistsFile;
  knnDistsFile.set_relation(relation);

  assert(boost::filesystem::is_directory(KNNPROTODIR));
  std::map<std::string, std::map<std::string, int> >& relationCache = knnDistsCache[relation];

  typedef std::pair<const string, std::map<std::string, int> > words_t;
  typedef std::pair<const string, int> related_t;

  BOOST_FOREACH(words_t& words, relationCache) {
    KnnDists::Word *word = knnDistsFile.add_word();
    word->set_word(words.first);
    BOOST_FOREACH(related_t& related, words.second) {
      word->add_related(related.first);
      word->add_distance(related.second);
    }
  }

  fstream out(protofile.c_str(), ios::out | ios::binary | ios::trunc);
  knnDistsFile.SerializeToOstream(&out);
  out.close();
}

void SimSynModule::loadKnnDistsCache() {
  BOOST_FOREACH(std::string &relation, rels) {
    std::string protofile = KNNPROTOFILE + pos + "_" + relation;
    if (ifstream(protofile.c_str(), std::ios::in).good()) {
      readProtobuf(relation);
    } else {
      std::cout << "note: computing the simsyn cache, next run will be faster." << std::endl;
      buildRelationCache(relation);
      writeProtobuf(relation);
    }
  }
}
