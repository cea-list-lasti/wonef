#include "MeroHoloModule.hpp"
#include "Paths.hpp"
#include "Timer.hpp"
#include "tools.h"
#include <iostream>
#include <fstream>



MeroHoloModule::MeroHoloModule(const std::string& datafile, int idModuleConf, int nIteration, bool _verbose) :
  TranslatorModule(_verbose), cntMeros(0), cntHolos(0) {

  loadMeroHolos(datafile);

  std::ostringstream oss;
  oss << idModuleConf << "." << nIteration;
  suffix = oss.str();
}

void MeroHoloModule::loadMeroHolos(std::string dataInput) {
  std::ifstream dataIfs(dataInput.c_str(), std::fstream::in);
  if (dataIfs.fail()) {
    std::cerr << "Oops, " << dataInput << " doesn't exist. " << __FILE__ << ":" << __LINE__ << std::endl;
    exit(-1);
  }
  std::string s;
  ulong cntMeros = 0;
  ulong cntHolos = 0;
  while (getline(dataIfs, s) ) {
    std::string str = s;
    std::string synsetId = s.substr(0, s.find(' '));

    std::string::const_iterator start, end;
    boost::match_results<std::string::const_iterator> what;
    boost::match_flag_type flags = boost::match_default;

    // extract meronyms && holonyms
    start = s.begin();
    end = s.end();
    while(regex_search(start, end, what, boost::regex("%p ([^ ]*) n"), flags))  {
      if (meronyms.find(synsetId)==meronyms.end()) {
        meronyms[synsetId]=std::set<std::string>();
      }
      meronyms[synsetId].insert(std::string(what[1].first, what[1].second));
      // update search position:
      start = what[1].second;
      // update flags:
      flags |= boost::match_prev_avail;
      flags |= boost::match_not_bob;
    }
    cntMeros+=meronyms[synsetId].size();

    while(regex_search(start, end, what, boost::regex("#p ([^ ]*) n"), flags))  {
      /*      std::cerr << "-------------------------------" << std::endl;
      std::cerr << "INSERT holo 0 : " << synsetId << " -> " << std::string(what[1].first, what[1].second) << std::endl;
      */

      holonyms[synsetId].insert(std::string(what[1].first, what[1].second));
      /*
      for (std::std::string it1 : reverseIndex[synsetId]) {
        std::cerr << it1 << std::endl;
      }
      std::cerr << "-->>>--" << std::endl;
      for (std::std::string it1 : reverseIndex[std::string(what[1].first, what[1].second)]) {
        std::cerr << it1 << std::endl;
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
  std::cerr << "Total Meronyms : " << cntMeros << std::endl;
  std::cerr << "Total Holonyms : " << cntHolos << std::endl;
  */
}

void MeroHoloModule::process(WORDNET::WordNet& wn) {
  /* Load TypeRoler files */
  meroTypeRoler = TypeRoler("COMPDUNOM");
  holoTypeRoler = TypeRoler("COMPDUNOM.reverse");


  // First compute the reverseIndex to know about existing instances
  for (const auto& itwn : wn) {
    std::string synsetId = itwn.first;
    WORDNET::WordNetEntry wne = itwn.second;
    for (const auto& itinstance : wne.frenchSynset) {
      std::string instance = itinstance.first;
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

int MeroHoloModule::get_cooc(MeroHoloMode m, const std::string& strA, ulong itlit_first_code) {
  TypeRoler& tr = m == MeroHoloMode::MERO ? meroTypeRoler : holoTypeRoler;
  for(auto p : tr.repository[strA]) {
    if (p.first == itlit_first_code) {
      return p.second;
    }
  }
  return 0;
}

// TODO: OPTIMIZE SCORE : MI ? Z ? or what ?

float MeroHoloModule::computeIsPartOfScore(const WORDNET::WordNet& wn, const std::string& strA, const std::string& strB) {
  if (wn.at(strB).frenchSynset.empty()) {
    return 0;
  }

  float sum = 0;
  if (verbose) {
    std::cerr << "--------------" << std::endl;
  }
  for (const auto& itlit : wn.at(strB).frenchSynset) {
    if (verbose) {
      std::cerr << strA << " is part of " << itlit.first  << " ? " << std::endl;
    }

    ulong itlit_first_code = meroTypeRoler.dicmapReverse[itlit.first];
    sum += get_cooc(MeroHoloMode::MERO, strA, itlit_first_code) /
      (float)(meroTypeRoler.numCoocs[strA] * holoTypeRoler.numCoocs[itlit.first]);

    //std::cerr << "MERO sum += " << get_cooc(MERO, strA, itlit_first_code) << " / (";
    //std::cerr << meroTypeRoler.numCoocs[strA] << "*" << holoTypeRoler.numCoocs[itlit.first] << ")\n";
    cntMeros++;
  }
  if (verbose) {
    std::cerr<< "Score : " << sum/(float)wn.at(strB).frenchSynset.size() << std::endl;
    std::cerr << "--------------" << std::endl;
  }

  return sum/(float)wn.at(strB).frenchSynset.size();
}


float MeroHoloModule::computeIsWholeOfScore(const WORDNET::WordNet& wn, const std::string& strA, const std::string& strB) {
  if (wn.at(strB).frenchSynset.empty()) {
    return 0;
  }

  float sum = 0;
  if (verbose) {
    std::cerr << "--------------" << std::endl;
  }
  for (const auto& itlit : wn.at(strB).frenchSynset) {
    if (verbose) {
      std::cerr << strA << " is whole of " << itlit.first  << " ? " << std::endl;
    }

    ulong itlit_first_code = holoTypeRoler.dicmapReverse[itlit.first];
    sum += get_cooc(MeroHoloMode::HOLO, strA, itlit_first_code) /
      (float)(meroTypeRoler.numCoocs[itlit.first] * holoTypeRoler.numCoocs[strA]);

    //std::cerr << "HOLO sum += " << get_cooc(HOLO, strA, itlit_first_code) << " / (";
    //std::cerr << meroTypeRoler.numCoocs[itlit.first] << "*" << meroTypeRoler.numCoocs[strA] << ")\n";
    cntHolos++;
  }

  if (verbose) {
    std::cerr<< "Score : " << sum/(float)wn.at(strB).frenchSynset.size() << std::endl;
    std::cerr << "--------------" << std::endl;
  }

  return sum/(float)wn.at(strB).frenchSynset.size();
}


std::string MeroHoloModule::trySelecAndReplace(const WORDNET::WordNet& wn, std::string synsetId, WORDNET::WordNetEntry& wne, std::pair<std::string, WORDNET::TgtCandidates> itlit) {
  std::set<std::pair<std::string, float> > elected;

  std::map<std::string, float> votes;

  for (auto itcand : itlit.second.cand) {
    std::string candidate = itcand.first;
    //std::cerr << "Processing : " << candidate << " - " << synsetId << std::endl;
    for (const std::string& synsetMeronym : meronyms[synsetId]) {
      wne.meros.insert(reverseIndex[synsetMeronym].begin(), reverseIndex[synsetMeronym].end());
      if (verbose) {
        std::cerr << "COMPUTE mero : " << candidate << " -> " << synsetMeronym << std::endl;
      }
      votes[candidate]+=computeIsWholeOfScore(wn, candidate, synsetMeronym);
    }
    if (verbose && votes[candidate]!=0) {
      std::cerr << "VOTE mero :" << votes[candidate] << std::endl;
    }

    for (std::string termHolonym : holonyms[synsetId]) {
      wne.holos.insert(reverseIndex[termHolonym].begin(), reverseIndex[termHolonym].end());
      if (verbose) {
        std::cerr << "COMPUTE holo : " << candidate << " -> " << termHolonym << std::endl;
      }
      votes[candidate]+=computeIsPartOfScore(wn, candidate, termHolonym);
    }
    if (verbose && votes[candidate]!=0) {
      std::cerr << "VOTE holo :" << votes[candidate] << std::endl;
    }
  }

  float bestVote = 0;
  std::pair<std::string, float> elec;
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
        wne.frenchSynset[itElec.first]=std::set<WORDNET::TranslationInfos>();
      }
      WORDNET::TranslationInfos translationInfos;
      translationInfos.original = itlit.first;
      translationInfos.processed = "meroholo" + suffix;
      translationInfos.score = itElec.second;
      wne.frenchSynset[itElec.first].insert(translationInfos);
    }
    return Dictionaries::definition[elected.begin()->first];
  }
  return "";
}


