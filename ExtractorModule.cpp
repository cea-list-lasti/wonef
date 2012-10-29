#include "ExtractorModule.hpp"
#include <cassert>

#include "levenshtein.hpp"
#include "tools.h"
#include "Tools.hpp"

ExtractorModule::ExtractorModule(std::string _pos, std::set<ExtractionType> _extractions)
  : pos(_pos), extractions(_extractions) { }

void ExtractorModule::process(WORDNET::WordNet& wn, bool /*verbose*/) {

  typedef std::pair<const std::string, WORDNET::WordNetEntry> wn_t;
  typedef std::pair<const std::string, WORDNET::TgtCandidates> candidate_t;
  typedef std::pair<const std::string, int> cand_t;

  std::map<std::string, int> WNIndex;

  /* Preprocessing: compute various informations our extraction rules need */
  for (wn_t& wnepair: wn) {
    for (candidate_t& candidates: wnepair.second.frenchCandidates) {
      std::string srcWord = candidates.first;
      // fill up WNIndex to know what words only appear in one synset
      WNIndex[srcWord]++;
    }
  }

  /* Actual code: setting candidate scores (promoting) and adding new instances */
  for (wn_t& wnepair: wn) {
    WORDNET::WordNetEntry& wne = wnepair.second;
    englishCount.clear();
    for (candidate_t& candidates: wne.frenchCandidates) {
      std::string srcWord = candidates.first;

      /* monosemous */
      if (WNIndex[srcWord] == 1) {
        if (extractions.count(ExtractionType::Monosemous) == 1) {
          for (cand_t& cand: candidates.second.cand) {
            std::string tgtWord = cand.first;
            /* If this word only appears in one synset, let's assume the
             * translation is correct since monosemous */
            addInstance(wne.frenchSynset, "monosemous", tgtWord, srcWord, 1);
          }
        }
      } else {
        switch (candidates.second.cand.size()) {
          /* no translation */
          case 0 :
            if (extractions.count(ExtractionType::NoTranslation) == 1) {
              if (candidates.second.capital) {
                // original == translation here.
                addInstance(wne.frenchSynset, "notranslation", srcWord, srcWord, 1);
              }
            }
            break;

            /* uniq */
          case 1 :
            /* If there's only one possible translation in this synset, choose it.
             * TODO: is this really helpful? for all part-of-speech? */
            if (extractions.count(ExtractionType::Uniq) == 1) {
              addInstance(wne.frenchSynset, "uniq", candidates.second.cand.begin()->first, srcWord, 1);
            }
            break;

            /* promote specifics */
          case 2 :
            // TODO does this have an effect at all? on how many synsets?
            /* We prefer more specific terms, eg. "individualité" is better
             * than "individual" */
            if (candidates.second.cand.begin()->first
                .find(candidates.second.cand.rbegin()->first) != string::npos
                && ((*candidates.second.cand.rbegin()).first.length()
                  - (*candidates.second.cand.begin()).first.length() > 2))
            {
              wne.frenchCandidates[srcWord].cand[candidates.second.cand.begin()->first]++;
            }
            else if (candidates.second.cand.rbegin()->first
                .find(candidates.second.cand.begin()->first) != string::npos
                && ((*candidates.second.cand.rbegin()).first.length() -
                  (*candidates.second.cand.begin()).first.length() > 2))
            {
              wne.frenchCandidates[srcWord].cand[candidates.second.cand.rbegin()->first]++;
            }


            /* promote close words */
          default:
            /* We're using Levenshtein distance to promote true and false
             * friends. This only affects the score of a candidate, and doesn't
             * add any new instance */

            // promote true (and false) fr/en friends
            for (map<string, int>::iterator itCand = candidates.second.cand.begin(); itCand!=candidates.second.cand.end(); itCand++) {
              int ldScore = 0;

              std::string candidate = itCand->first;
              if (pos == "verb") {
                // compute the score without the pronoun
                candidate = candidates.second.verbCand[candidate];
              }

              /* Desaccentuation: using the faster method, which has been
               * corrected using the Unicode-aware one. */
              ldScore = levenshtein(fastDesax(desaxData, candidate), srcWord);
              //ldScore = Dist.LD(fastDesax(desaxData, candidate), srcWord);
              // ldScore = levenshtein(desaxUTF8(candidate), srcWord);

              if (ldScore<=3) {
                wne.frenchCandidates[srcWord].cand[itCand->first]+=3-ldScore;
              }
            }
        }
      }

      /* For this synset, we read every possible verb and its candidate
       * translations. We can now look at repeated translations (eg. two
       * english verbs give the same french word) and add them to our list.
       *
       * The plan is to build something like an inverted index and to see which
       * french word lead to multiple english words */

      for (cand_t& cand: candidates.second.cand) {
        std::string tgtWord = cand.first;
        englishCount[tgtWord].insert(srcWord);
      }
    }

    /* multiplesource */
    typedef std::pair<const std::string, std::set<std::string> > count_t;

    if (extractions.count(ExtractionType::MultipleSource) == 1) {
      for (count_t& count: englishCount) {
        if(count.second.size() > 1) {
          for (const std::string& srcWord: count.second) {
            /* score : number of candidates by source word in the synset
             * (the more candidates there are, the less they are meaningful)*/
            int score = wne.nbCandidates / wne.frenchCandidates.size();
            addInstance(wne.frenchSynset, "multiplesource", count.first, srcWord, score);
          }
        }
      }
    }
  }
}

void ExtractorModule::addInstance(std::map<std::string, std::set<WORDNET::TranslationInfos> >& frenchSynset,
    const std::string& processed, const std::string& translation,
    const std::string& original, int score) {

  WORDNET::TranslationInfos translationInfos;
  translationInfos.original = original;
  translationInfos.score = score;
  translationInfos.processed = processed;

  if (frenchSynset.find(translation) == frenchSynset.end()) {
    frenchSynset[translation] = set<WORDNET::TranslationInfos>();
  }

  frenchSynset[translation].insert(translationInfos);
}

ExtractionType ExtractorModule::fromInt(int n) {
  switch(n) {
    case 1: return ExtractionType::Monosemous;
    case 2: return ExtractionType::NoTranslation;
    case 3: return ExtractionType::Uniq;
    case 4: return ExtractionType::MultipleSource;
    default: assert(false);
  }
}

std::string ExtractorModule::toString(ExtractionType e) {
  switch(e) {
    case ExtractionType::Monosemous:     return "monosemous";
    case ExtractionType::NoTranslation:  return "notranslation";
    case ExtractionType::Uniq:           return "uniq";
    case ExtractionType::MultipleSource: return "multiplesource";
    default: assert(false);
  }
}


