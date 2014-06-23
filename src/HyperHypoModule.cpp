#include "HyperHypoModule.hpp"
#include "Loader.hpp"
#include "levenshtein.hpp"
#include "Paths.hpp"
#include "tools.h"
#include <iostream>
#include <fstream>
#include <limits>
#include "boost/regex.hpp"



HyperHypoModule::HyperHypoModule(std::string dataInput, std::string typeroler, TRMode _mode, std::string _pos, int idModuleConf, int nIteration, bool _verbose)
  : TranslatorModule(_verbose), mode(_mode), pos(_pos) {

    tRoler = TypeRoler(typeroler);

    loadHyperHypos(dataInput);
    std::ostringstream oss;
    oss << idModuleConf << "." << nIteration;
    suffix = oss.str();
  }


HyperHypoModule::~HyperHypoModule() {
  std::cerr << "Deleting hypernyms " << std::endl;
  std::cerr << "Deleting hyponyms " << std::endl;
  hypernyms.clear();
  hyponyms.clear();
}


void HyperHypoModule::loadHyperHypos(std::string dataInput) {
  std::ifstream dataIfs(dataInput.c_str(), std::fstream::in);
  if (dataIfs.fail()) {
    std::cerr << "Oops, " << dataInput << " doesn't exist. " << __FILE__ << ":" << __LINE__ << std::endl;
    exit(-1);
  }
  std::string s;
  int cntHypers = 0;
  int cntHypos = 0;
  while (getline(dataIfs, s) ) {
    std::string str = s;
    std::string synsetId = s.substr(0, s.find(' '));

    std::string::const_iterator start, end;
    boost::match_results<std::string::const_iterator> what;
    boost::match_flag_type flags = boost::match_default;

    // extract hypernym && hyponyms
    start = s.begin();
    end = s.end();
    while(regex_search(start, end, what, boost::regex("@ ([^ ]*) "), flags))  {
      hypernyms[synsetId].insert(std::string(what[1].first, what[1].second));
      // update search position:
      start = what[1].second;
      // update flags:
      flags |= boost::match_prev_avail;
      flags |= boost::match_not_bob;
    }
    cntHypers+=hypernyms[synsetId].size();

    while(regex_search(start, end, what, boost::regex("~ ([^ ]*) "), flags))  {
      hyponyms[synsetId].insert(std::string(what[1].first, what[1].second));
      // update search position:
      start = what[1].second;
      // update flags:
      flags |= boost::match_prev_avail;
      flags |= boost::match_not_bob;
    }
    cntHypos+=hyponyms[synsetId].size();
  }

  dataIfs.close();


  std::cerr << "Total Hyperonyms : " << cntHypers << std::endl;
  std::cerr << "Total Hyponyms : " << cntHypos << std::endl;
}



void HyperHypoModule::process(WORDNET::WordNet& wn){
  std::map<std::string, std::set<std::string> >reverseIndex;

  int nbDisamb = 0;

  for (std::map<std::string, WORDNET::WordNetEntry>::iterator itwn = wn.begin(); itwn !=wn.end(); itwn++) {
    for (std::map<std::string, std::set<WORDNET::TranslationInfos> >::iterator itwne = itwn->second.frenchSynset.begin(); itwne !=itwn->second.frenchSynset.end(); itwne++) {
      reverseIndex[itwn->first].insert(itwne->first);
    }
  }
  for (std::map<std::string, WORDNET::WordNetEntry>::iterator itwn = wn.begin(); itwn !=wn.end(); itwn++) {
    for (std::map<std::string, WORDNET::TgtCandidates >::iterator it = itwn->second.frenchCandidates.begin(); it != itwn->second.frenchCandidates.end(); it++) {
      float best = 0;
      std::string elected = "";
      for (std::map<std::string, int>::iterator itCand = it->second.cand.begin(); itCand != it->second.cand.end(); itCand++) {
        int validSumHypo = 0;
        int validSumHyper = 0;
        float sum = 0;
        std::string head = "";
        /*if(hyponyms[itwn->first].size()==0) {
            validSumHypo+=2;
            }
         */


        for(std::set<std::string>::iterator itHypos = hyponyms[itwn->first].begin(); itHypos != hyponyms[itwn->first].end(); itHypos++) {
          for (std::set<std::string>::iterator itSyn = reverseIndex[*itHypos].begin(); itSyn !=  reverseIndex[*itHypos].end(); itSyn++) {
            validSumHypo++;
            itwn->second.hypos.insert(*itSyn);
            if (head=="") {
              head = getHead(*itSyn);
            }
            float score = - std::numeric_limits<float>::infinity();;
            if (pos == "noun") {
              score = tRoler.computeIsAScore( itCand->first, head, R_HYPO);
            } else if (pos == "verb") {
              // compute the score without the pronoun
              score = tRoler.computeIsAScore( it->second.verbCand[itCand->first], head, R_HYPO);
            }

            if( verbose) {
              std::cerr << "DEBUG "<<" : " << it->first << " : " << itCand->first << " > " << *itSyn << " : " << score << std::endl;
            }
            if (std::isfinite(score)) {
              if( verbose) {
                std::cerr << "DEBUG hyponyms"<<" : " << it->first << " : " << itCand->first << " > " << *itSyn << " : " << score << std::endl;
              }
              // reduce weight of hyponyms of the synset among the candidates
              sum+=score==1?0.3:score;
            } else {
              // hyponym not considered in the sum
              validSumHypo --;
            }
            if (score==0 && head[head.length()-1]=='s') {
              head=head.substr(0, head.length()-1);
              itSyn--;
              validSumHypo --;
            } else {
              head="";
            }
          }
        }

        /* if (hypernyms[itwn->first].size()==0) {
            validSumHyper+=2;
            }
         */

        for(std::set<std::string>::iterator itHyper = hypernyms[itwn->first].begin(); itHyper != hypernyms[itwn->first].end(); itHyper++) {
          for (std::set<std::string>::iterator itSyn = reverseIndex[*itHyper].begin(); itSyn !=  reverseIndex[*itHyper].end(); itSyn++) {
            validSumHyper++;
            itwn->second.hypers.insert(*itSyn);
            if (head=="") {
              head = getHead(*itSyn);
            }
            float score = - std::numeric_limits<float>::infinity();
            if (pos == "noun") {
              score = tRoler.computeIsAScore(head, itCand->first, R_HYPER);
            } else if (pos == "verb") {
              // compute the score without the pronoun
              score = tRoler.computeIsAScore(head, it->second.verbCand[itCand->first], R_HYPER);
            }
            if (std::isfinite(score)) {
              if (verbose){
                std::cerr << "DEBUG hypernyms : " << it->first << " : " << itCand->first << " < " << *itSyn << " : " << score << std::endl;
              }
              // reduce weight of hypernyms of the synset among the candidates
              sum+=score==1?0.3:score;
            } else {
              // hypernym not considered in the sum
              validSumHyper --;
            }
            if (score == 0 && head[head.length()-1]=='s') {
              head=head.substr(0, head.length()-1);
              itSyn--;
              validSumHyper --;
            } else {
              head="";
            }
          }
        }

        /*
           if (levenshtein(desaxUTF8(itCand->first),it->first)<=3) {
           sum*=1.+ (1.-0.2*lDist.LD(desaxUTF8(itCand->first),it->first));
           }
         */
        sum =  sum / (validSumHypo + validSumHyper);
        if (verbose && sum >0 ) {
          std::cerr << itwn->first<<":"<<it->first << " -> " << itCand->first << ":" << sum << std::endl;
        }
        if (validSumHypo>1 && validSumHyper > 1 && sum> best) {
          best = sum;
          elected = itCand->first;
        }

        /* Even if it was not kept, add the candidate to our list of "scores" */
        if (sum > 0) {
          WORDNET::TranslationInfos translationCandidate(it->first, "hyperhypo" + suffix, sum);
          itwn->second.frenchScore[itCand->first].insert(translationCandidate);
        }

      }
      /* if (validSumHypo<2 || validSumHypo<2) {
          elected="";
          }
       */
      if (verbose) {
        std::cerr << "elected : "<< it->first << " => " << elected << std::endl;
      }

      if (elected!="") {
        if (itwn->second.frenchSynset.find(elected)==itwn->second.frenchSynset.end()) {
          itwn->second.frenchSynset[elected] = std::set<WORDNET::TranslationInfos>();
        }
        WORDNET::TranslationInfos translationInfos;
        translationInfos.original = it->first;
        translationInfos.processed = "hyperhypo" + suffix;
        translationInfos.score = best;
        itwn->second.frenchSynset[elected].insert(translationInfos);
        itwn->second.newdef=Dictionaries::definition[elected];
        nbDisamb++;
      }
    }



  }

}





std::string HyperHypoModule::getHead (std::string locution) {
  std::string endLoc = locution;
  if (LoaderModule::posList.find(locution)==LoaderModule::posList.end()) {
    return locution;
  }

  std::string partialLoc = endLoc.substr(0, endLoc.find('_'));
  while (LoaderModule::posList.find(partialLoc)==LoaderModule::posList.end()
      && partialLoc!=endLoc
      && partialLoc!=endLoc.substr(0, endLoc.length()-1)) {
    endLoc = endLoc.substr(endLoc.find('_')+1);
    partialLoc = endLoc.substr(0, endLoc.find('_'));
    if (partialLoc[partialLoc.length()-1]=='s'
        ||partialLoc[partialLoc.length()-1]=='x') {
      if (LoaderModule::posList.find(partialLoc)==LoaderModule::posList.end()) {
        partialLoc=partialLoc.substr(0, partialLoc.length()-1);
      }
    }
  }
  std::string head = partialLoc;

  return head;
}
