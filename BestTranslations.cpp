#include "BestTranslations.hpp"
#include <string>
#include <iostream>
#include "boost/regex.hpp"

BestTranslations::BestTranslations() {
}

BestTranslations::~BestTranslations() {
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
      int nbNotrans = 0;

      for (std::set<WORDNET::TranslationInfos>::iterator
	   itTrans = itFrench->second.begin();
	   itTrans != itFrench->second.end(); itTrans ++) {

	nbTranslations++;
	if (itTrans->processed == "uniq") {
	  nbUniq++;
	} else if (itTrans->processed == "monosemous"){
	  nbMono++;
	} else if (itTrans->processed == "notranslation"){
	  nbNotrans++;
	}

      } // end itTrans

      /* Keep in the french synset only the translations
	  - that come from "uniq", "monosemous" or "notranslation"
	  - that have been chosen by several modules
      ------------------------------------------------------------------------*/
      if (nbTranslations < 2 && nbUniq == 0 /*&& nbMono == 0 && nbNotrans == 0*/) {
	itEntry->second.frenchSynset.erase(itFrench++);

      } else {
	itFrench++;
      }

    } // end itFrench

  } //end itEntry

}