#include "BestTranslations.hpp"

#include "Options.hpp"

#include <string>
#include <iostream>
#include "boost/regex.hpp"

BestTranslations::BestTranslations(const Options& _options, bool _easyFilter)
  : options(_options), easyFilter(_easyFilter) {
}

void BestTranslations::choose(WORDNET::WordNet& wn) {

  for (std::map<std::string, WORDNET::WordNetEntry>::iterator
       itEntry = wn.begin(); itEntry != wn.end(); itEntry++) {

    for (std::map<std::string, std::set<WORDNET::TranslationInfos> >::iterator
         itFrench = itEntry->second.frenchSynset.begin();
         itFrench != itEntry->second.frenchSynset.end();) {

      /* Count for each french word of the synset :
          - the number of translations that gave this french word as a result ;
          - the number of "uniq", "monosemous" or "notranslation" among those
          translations.
      ------------------------------------------------------------------------*/

      int nbTranslations = 0;
      int nbUniq = 0;
      int nbMono = 0;
      int nbLevenshtein = 0;
      int nbNotrans = 0;
      int nbMultiple = 0;
      int scoreMultiple = 0;

      for (std::set<WORDNET::TranslationInfos>::iterator
           itTrans = itFrench->second.begin();
           itTrans != itFrench->second.end(); itTrans ++) {

        nbTranslations++;
        if (itTrans->processed == "uniq") {
          nbUniq++;
        } else if (itTrans->processed == "monosemous") {
          nbMono++;
        } else if (itTrans->processed == "notranslation") {
          nbNotrans++;
        } else if (itTrans->processed == "levenshtein") {
          nbLevenshtein++;
        } else if (itTrans->processed == "multiplesource") {
          /* Several "multiplesource" instances are counted like a single one
             (so that the french word can be deleted if no other translation exists)
          --------------------------------------------------------------------*/
          if (nbMultiple > 0) {
            nbTranslations--;
          }
          // the score isn't really a score: that's the number of candidates
          // per instance in our synset. It's the same for every instance.
          scoreMultiple = itTrans->score;
          nbMultiple++;
        }

      } // end itTrans

      /* Check if the "multiplesource" word is reliable on his own :
          - old scoreMultiple : nb of candidates by source word in the synset
             (less is better)
          - new scoreMultiple : take into account of the nb of multiples (2, 3,
            4...) (more is better)
      ------------------------------------------------------------------------*/
      bool goodMultiple;
      /* If we're not looking for precision, we're taking the word anyway */
      goodMultiple = options.mode != Mode::Precision;

      if (nbMultiple != 0) {
        scoreMultiple = scoreMultiple / (float) nbMultiple;
        if (scoreMultiple >= 3) {
          goodMultiple = true;
        }
      }

      /* Keep in the french synset only the translations
          - that come from "uniq"
          - that have a good "multiplesource" score
          - that have been chosen by several modules
      ------------------------------------------------------------------------*/
      bool keepit = false;
      if (easyFilter && (nbUniq > 0 || nbLevenshtein > 0 || nbMono > 0)) {
        std::cout << "keep it true" << std::endl;
        keepit = true;
      } else if (nbTranslations >= 2 || nbUniq > 0 || goodMultiple /*|| nbMono > 0 || nbNotrans > 0*/) {
        keepit = true;
      }

      if (keepit) {
        itFrench++;
      } else {
        itEntry->second.frenchSynset.erase(itFrench++);
      }

    } // end itFrench

  } //end itEntry

}
