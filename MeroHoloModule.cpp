#include "MeroHoloModule.hpp"
#include "distance.hpp"
#include "Paths.hpp"
#include "../src/tools.h"

#include <iostream>
#include <fstream>

using namespace std;

MeroHoloModule::MeroHoloModule(string& datafile, int idModuleConf, int nIteration, bool _verbose) :
  TranslatorModule(_verbose), cntMeros(0), cntHolos(0) {
  initializeDicMap(dicmap, WORDS_IDS);
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

void MeroHoloModule::processLine(ulong currentId, string s, Mode mode) {
  stringstream ss;
  ss << s;
  string currentIdCooc;
  uint nbOccs = 0;
  uint sum = 0;
  while (ss.get()!='}') {
    ss.unget();
    ulong currentIdCooc;
    ss >> currentIdCooc;
    ss.ignore(16, ',');
    ss >> nbOccs;
    ss.ignore(4, ';');
    sum+=nbOccs;
    if (mode==MERO) {
      coocsMero[dicmap[currentId]].insert(make_pair(dicmap[currentIdCooc], nbOccs));
    } else if (mode==HOLO) {
      coocsHolo[dicmap[currentId]].insert(make_pair(dicmap[currentIdCooc], nbOccs));
    }
  }

  if (mode==MERO) {
    sumMeros[dicmap[currentId]]=sum;
  } else if (mode==HOLO) {
    sumHolos[dicmap[currentId]]=sum;
  }

}

MeroHoloModule::~MeroHoloModule() {
}

void MeroHoloModule::finalize() {
  meronyms.clear();
  holonyms.clear();
  coocsMero.clear();
  coocsHolo.clear();
  sumMeros.clear();
  sumHolos.clear();
}

void MeroHoloModule::process(WORDNET::WordNet& wn) {

  /* Load TypeRoler files */
  ifstream idss(TYPEROLERFILE, fstream::in);
  if (idss.fail()) {
    cerr << "Oops, " << TYPEROLERFILE << " doesn't exist. " << __FILE__ << ":" << __LINE__ << endl;
    exit(-1);
  }
  string s;
  cerr << "Loading " << TYPEROLERFILE << endl;
  while (getline(idss, s) ) {
    ulong currentId;
    stringstream ss;
    ss << s;
    ss >> currentId;
    if (s.find('}') - s.find('{') !=1 && s.find(']')-s.find('[')!=3) {
      processLine(currentId, s.substr(s.find('{')+1), MERO);
    }
  }
  idss.close();
  cout << "end load TYPEROLERFILE" << endl;

  idss.open(HOLOFILE);

  cerr << "Loading " << HOLOFILE << endl;
  while (getline(idss, s) ) {
    ulong currentId;
    stringstream ss;
    ss << s;
    ss >> currentId;
    if (s.find('}') - s.find('{') !=1 && s.find(']')-s.find('[')!=3) {
      processLine(currentId, s.substr(s.find('{')+1), HOLO);
    }
  }
  idss.close();
  cout << "end load HOLOFILE" << endl;




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
    sum += (float)coocsMero[strA][itlit.first]/(float)(sumMeros[strA]*sumHolos[itlit.first]);
    // cerr << "MERO sum += " << coocsMero[strA][itlit.first] << " / (";
    // cerr << sumMeros[strA] << "*" << sumHolos[itlit.first] << ")\n";
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
    sum += (float)coocsHolo[strA][itlit.first]/(float)(sumMeros[itlit.first]*sumHolos[strA]);
    // cerr << "HOLO sum += " << coocsHolo[strA][itlit.first] << " / (";
    // cerr << sumMeros[itlit.first] << "*" << sumHolos[strA] << ")\n";
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
    // cerr << "Processing : " << candidate << " - " << synsetId << endl;
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

    for (std::string synsetHolonym : holonyms[synsetId]) {
      wne.holos.insert(reverseIndex[synsetHolonym].begin(), reverseIndex[synsetHolonym].end());
      if (verbose) {
        cerr << "COMPUTE holo : " << candidate << " -> " << synsetHolonym << endl;
      }
      votes[candidate]+=computeIsPartOfScore(wn, candidate, synsetHolonym);
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

