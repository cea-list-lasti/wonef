#include "MeroHoloModule.hpp"
#include "Paths.hpp"
#include "Timer.hpp"
#include "../src/tools.h"

#include <iostream>
#include <fstream>

using namespace std;

MeroHoloModule::MeroHoloModule(string& datafile, int idModuleConf, int nIteration, bool _verbose) :
  TranslatorModule(_verbose), cntMeros(0), cntHolos(0) {

  loadMeroHolos(datafile);

  std::ostringstream oss;
  oss << idModuleConf << "." << nIteration;
  suffix = oss.str();
}

void MeroHoloModule::loadMeroHolos(string dataInput) {
  ifstream dataIfs(dataInput.c_str(), fstream::in);
  if (dataIfs.fail()) {
    cerr << "Oops, " << dataInput << " doesn't exist. " << __FILE__ << ":" << __LINE__ << endl;
    exit(-1);
  }
  string s;
  ulong cntMeros = 0;
  ulong cntHolos = 0;
  while (getline(dataIfs, s) ) {
    string str = s;
    string synsetId = s.substr(0, s.find(' '));

    std::string::const_iterator start, end;
    boost::match_results<std::string::const_iterator> what;
    boost::match_flag_type flags = boost::match_default;

    // extract meronyms && holonyms
    start = s.begin();
    end = s.end();
    while(regex_search(start, end, what, boost::regex("%p ([^ ]*) n"), flags))  {
      if (meronyms.find(synsetId)==meronyms.end()) {
        meronyms[synsetId]=set<string>();
      }
      meronyms[synsetId].insert(string(what[1].first, what[1].second));
      // update search position:
      start = what[1].second;
      // update flags:
      flags |= boost::match_prev_avail;
      flags |= boost::match_not_bob;
    }
    cntMeros+=meronyms[synsetId].size();

    while(regex_search(start, end, what, boost::regex("#p ([^ ]*) n"), flags))  {
      /*      cerr << "-------------------------------" << endl;
      cerr << "INSERT holo 0 : " << synsetId << " -> " << string(what[1].first, what[1].second) << endl;
      */

      holonyms[synsetId].insert(string(what[1].first, what[1].second));
      /*
      for (std::string it1 : reverseIndex[synsetId]) {
        cerr << it1 << endl;
      }
      cerr << "-->>>--" << endl;
      for (std::string it1 : reverseIndex[string(what[1].first, what[1].second)]) {
        cerr << it1 << endl;
      }
      */

      // update search position:
      start = what[1].second;
      // update flags:
      flags |= boost::match_prev_avail;
      flags |= boost::match_not_bob;
    }
    cntHolos+=holonyms[synsetId].size();
  }

  dataIfs.close();
  /*
  cerr << "Total Meronyms : " << cntMeros << endl;
  cerr << "Total Holonyms : " << cntHolos << endl;
  */
}

void MeroHoloModule::process(WORDNET::WordNet& wn) {
  /* Load TypeRoler files */
  meroTypeRoler = TypeRoler(TYPEROLERFILE, "COMPDUNOM");
  holoTypeRoler = TypeRoler(HOLOFILE, "COMPDUNOM.reverse");


  // First compute the reverseIndex to know about existing instances
  for (const auto& itwn : wn) {
    string synsetId = itwn.first;
    WORDNET::WordNetEntry wne = itwn.second;
    for (const auto& itinstance : wne.frenchSynset) {
      string instance = itinstance.first;
      reverseIndex[synsetId].insert(instance);
    }
  }

  // Then try to add new instances with trySelectAndReplace
  for (auto& itwn : wn) {
    WORDNET::WordNetEntry& wne = itwn.second;
    for (auto& itlit : wne.frenchCandidates) {
      auto candidates = itlit.second.cand;
      if (!candidates.empty()) {
        wne.newdef = trySelecAndReplace(wn, itwn.first, wne, itlit);
      }
    }
  }
}

int MeroHoloModule::get_cooc(Mode m, const string& strA, ulong itlit_first_code) {
  TypeRoler& tr = m == MERO ? meroTypeRoler : holoTypeRoler;
  for(auto p : tr.repository[strA]) {
    if (p.first == itlit_first_code) {
      return p.second;
    }
  }
  return 0;
}

// TODO: OPTIMIZE SCORE : MI ? Z ? or what ?

float MeroHoloModule::computeIsPartOfScore(const WORDNET::WordNet& wn, const string& strA, const string& strB) {
  if (wn.at(strB).frenchSynset.empty()) {
    return 0;
  }

  float sum = 0;
  if (verbose) {
    cerr << "--------------" << endl;
  }
  for (const auto& itlit : wn.at(strB).frenchSynset) {
    if (verbose) {
      cerr << strA << " is part of " << itlit.first  << " ? " << endl;
    }

    ulong itlit_first_code = meroTypeRoler.dicmapReverse[itlit.first];
    sum += get_cooc(MERO, strA, itlit_first_code) /
      (float)(meroTypeRoler.numCoocs[strA] * holoTypeRoler.numCoocs[itlit.first]);

    //cerr << "MERO sum += " << get_cooc(MERO, strA, itlit_first_code) << " / (";
    //cerr << meroTypeRoler.numCoocs[strA] << "*" << holoTypeRoler.numCoocs[itlit.first] << ")\n";
    cntMeros++;
  }
  if (verbose) {
    cerr<< "Score : " << sum/(float)wn.at(strB).frenchSynset.size() << endl;
    cerr << "--------------" << endl;
  }

  return sum/(float)wn.at(strB).frenchSynset.size();
}


float MeroHoloModule::computeIsWholeOfScore(const WORDNET::WordNet& wn, const string& strA, const string& strB) {
  if (wn.at(strB).frenchSynset.empty()) {
    return 0;
  }

  float sum = 0;
  if (verbose) {
    cerr << "--------------" << endl;
  }
  for (const auto& itlit : wn.at(strB).frenchSynset) {
    if (verbose) {
      cerr << strA << " is whole of " << itlit.first  << " ? " << endl;
    }

    ulong itlit_first_code = holoTypeRoler.dicmapReverse[itlit.first];
    sum += get_cooc(HOLO, strA, itlit_first_code) /
      (float)(meroTypeRoler.numCoocs[itlit.first] * holoTypeRoler.numCoocs[strA]);

    //cerr << "HOLO sum += " << get_cooc(HOLO, strA, itlit_first_code) << " / (";
    //cerr << meroTypeRoler.numCoocs[itlit.first] << "*" << meroTypeRoler.numCoocs[strA] << ")\n";
    cntHolos++;
  }

  if (verbose) {
    cerr<< "Score : " << sum/(float)wn.at(strB).frenchSynset.size() << endl;
    cerr << "--------------" << endl;
  }

  return sum/(float)wn.at(strB).frenchSynset.size();
}


string MeroHoloModule::trySelecAndReplace(const WORDNET::WordNet& wn, string synsetId, WORDNET::WordNetEntry& wne, pair<string, WORDNET::TgtCandidates> itlit) {
  set<pair<string, float> > elected;

  map<string, float> votes;

  for (auto itcand : itlit.second.cand) {
    std::string candidate = itcand.first;
    //cerr << "Processing : " << candidate << " - " << synsetId << endl;
    for (const std::string& synsetMeronym : meronyms[synsetId]) {
      wne.meros.insert(reverseIndex[synsetMeronym].begin(), reverseIndex[synsetMeronym].end());
      if (verbose) {
        cerr << "COMPUTE mero : " << candidate << " -> " << synsetMeronym << endl;
      }
      votes[candidate]+=computeIsWholeOfScore(wn, candidate, synsetMeronym);
    }
    if (verbose && votes[candidate]!=0) {
      cerr << "VOTE mero :" << votes[candidate] << endl;
    }

    for (std::string termHolonym : holonyms[synsetId]) {
      wne.holos.insert(reverseIndex[termHolonym].begin(), reverseIndex[termHolonym].end());
      if (verbose) {
        cerr << "COMPUTE holo : " << candidate << " -> " << termHolonym << endl;
      }
      votes[candidate]+=computeIsPartOfScore(wn, candidate, termHolonym);
    }
    if (verbose && votes[candidate]!=0) {
      cerr << "VOTE holo :" << votes[candidate] << endl;
    }
  }

  float bestVote = 0;
  pair<string, float> elec;
  for(auto itVotes : votes) {
    std::string candidate = itVotes.first;
    float vote = itVotes.second;
    if (vote > bestVote && !candidate.empty()) {
      bestVote = vote;
      elec = itVotes;
    }
  }

  if (!elec.first.empty()) {
    elected.insert(elec);
    for (auto itElec : elected) {
      if (wne.frenchSynset.find(itElec.first) == wne.frenchSynset.end()) {
        wne.frenchSynset[itElec.first]=set<WORDNET::TranslationInfos>();
      }
      WORDNET::TranslationInfos translationInfos;
      translationInfos.original = itlit.first;
      translationInfos.processed = "meroholo" + suffix;
      translationInfos.score = itElec.second;
      wne.frenchSynset[itElec.first].insert(translationInfos);
    }
    return LoaderModule::tgt2TgtDefs[elected.begin()->first];
  }
  return "";
}


