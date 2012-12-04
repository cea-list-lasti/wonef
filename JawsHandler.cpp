#include "JawsHandler.hpp"
#include "Tools.hpp"

#include <iostream>
#include <iomanip>
#include <boost/lexical_cast.hpp>
#include <math.h>
#include <unicode/unistr.h>
#include <unicode/stringpiece.h>

JawsHandler::JawsHandler(std::ofstream& _out,
    std::set<std::string>& _polyLitList,
    std::set<std::string>& _polyIdsList,
    std::map<std::string, std::set<std::string> >& _vtNet,
    std::map<std::string, std::set<std::string> >& _vtNetIdIdent,
    std::map<std::pair<std::string, std::string>, int>& _goldValue,
    bool _gold, std::string _pos,
    const std::map<std::string, int>& _BCS,
    const std::map<int, int>& _BCSCount) :
  out(_out),
  nbSynsets(0), nbJawsSynsets(0), nbGtSynsets(0),
  nbOriginals(0), nbPolyOriginals(0), nbBcsOriginals(0),
  nbTermsInJaws(0), nbPolyTermsInJaws(0), nbBcsTermsInJaws(0),
  nbTermsInGt(0), nbPolyTermsInGt(0), nbBcsTermsInGt(0),
  nbTermsInGtAndAJawsSynset(0), nbPolyTermsInGtAndAJawsSynset(0), nbBcsTermsInGtAndAJawsSynset(0),
  nbTermsOk(0), nbPolyTermsOk(0), nbBcsTermsOk(0),
  nbInJawsSynsetInGt(0), nbPolyInJawsSynsetInGt(0), nbBcsInJawsSynsetInGt(0),
  totalPercentageTermsOkInSynset(0.0f), totalPercentagePolyTermsOkInSynset(0.0f), totalPercentageBcsTermsOkInSynset(0),
  polyLitList(_polyLitList), polyIdsList(_polyIdsList),
  vtNet(_vtNet), vtNetIdIdent(_vtNetIdIdent),
  goldValue(_goldValue), gold (_gold), pos(_pos), BCS(_BCS), BCSCount(_BCSCount),
  totalType2(0) {

  candidates = std::map<std::string, std::set<std::string > >();

  if(gold) {
    for (auto itGold : goldValue) {
      goldIds.insert((itGold.first).first);
    }
  }

  BCSJawsCount = { {1, 0}, {2, 0}, {3, 0} };
}

std::string JawsHandler::get_attr(const xmlpp::SaxParser::AttributeList& attrs, std::string name) {
  for(const xmlpp::SaxParser::Attribute& attr: attrs) {
    if (attr.name == name) return attr.value;
  }
  exit(-1);
}

void JawsHandler::on_start_element(const std::string& name, const xmlpp::SaxParser::AttributeList& attrs) {

  tmpString.clear();
  tmpString.str(std::string());

  if(name == "SYNSET") {
    id = get_attr(attrs, "id");

    nbTermsOkInSynset = 0;
    nbPolyTermsOkInSynset = 0;
    nbTermsInSynset = 0;
    nbPolyTermsInSynset = 0;


  } else if(name == "CANDIDATES") {
    original = get_attr(attrs, "original");

    wne.frenchCandidates[original] = WORDNET::TgtCandidates();

  } else if (name == "INSTANCES") {
    translation = get_attr(attrs, "translation");
    wne.frenchSynset[translation] = std::set<WORDNET::TranslationInfos>();

  } else if(name == "INSTANCE") {
    WORDNET::TranslationInfos transInfos;
    transInfos.original = get_attr(attrs, "original");
    transInfos.processed = get_attr(attrs, "processed");
    std::string score = get_attr(attrs, "score");
    transInfos.score = boost::lexical_cast<float>(score);
    wne.frenchSynset[translation].insert(transInfos);
    // MWE expressions don't count for now
    //if (translation.find("_") == std::string::npos) {
    //  std::cout << "WONEF:" << id << ":" << translation << std::endl;
    //}
  }

}


void JawsHandler::on_characters(const std::string& characters) {
  tmpString << characters;
}

bool gtHasTranslation(std::set<std::string> gtTerms, std::string jawsTerm) {
  UnicodeString unicodeJawsTerm = UnicodeString::fromUTF8(StringPiece(jawsTerm));

  for(std::string correct: gtTerms) {
    UnicodeString unicodeCorrect = UnicodeString::fromUTF8(StringPiece(correct));
    if (unicodeCorrect.caseCompare(unicodeJawsTerm, 0) == 0) {
      return true;
    }
  }

  return false;
}


void JawsHandler::on_end_element(const std::string &name){

   if(name == "ORIGINALDEF") {
    wne.def = tmpString.str();

   } else if(name == "CANDIDATES") {
    nbOriginals++;
    if (polyLitList.find(original) != polyLitList.end()) {
      nbPolyOriginals++;
    }
    if (BCS.find(id) != BCS.end()) {
      nbBcsOriginals++;
    }
    original = std::string();

  } else if (name == "INSTANCES") {
    /* /!\ MWE expressions don't count for now
     * But we would want them to!
     */
    if (translation.find("_") != std::string::npos) { return; }

    // check if the translation comes from polysemous source terms
    bool polysemous = false;
    for (WORDNET::TranslationInfos itOrig : wne.frenchSynset[translation]) {
      if (polyLitList.find(itOrig.original) != polyLitList.end()) {
        polysemous = true;
      }
    }

    // check if the translation comes from a BCS synset
    bool bcs = BCS.find(id) != BCS.end();

    // count terms in Jaws: all, polysemous, bcs
    nbTermsInJaws++;
    nbTermsInSynset++;
    if (polysemous) {
      nbPolyTermsInJaws++;
      nbPolyTermsInSynset++;
    }
    if (bcs) {
      nbBcsTermsInJaws++;
      nbBcsTermsInSynset++;
    }

    // count terms in Jaws which are in synsets translated in GT
    if (vtNetIdIdent[id].size() > 0) {
      nbInJawsSynsetInGt++;
      if (polysemous) {
        nbPolyInJawsSynsetInGt++;
      }
      if (bcs) {
        nbBcsInJawsSynsetInGt++;
      }

      // count terms in the same synset in JAWS and GT
      if (gtHasTranslation(vtNetIdIdent[id], translation)) {
        nbTermsOk++;
        nbTermsOkInSynset++;
        if (polysemous) {
          nbPolyTermsOk++;
          nbPolyTermsOkInSynset++;
        }
        if (bcs) {
          nbBcsTermsOk++;
          nbBcsTermsOkInSynset++;
        }
      }
    }
    jawsNet[translation].insert(id);
    jawsNetIdIdent[id].insert(translation);
    translation = std::string();

  } else if(name == "SYNSET") {
    bool transInGt = false;
    bool transInJaws = false;
    nbSynsets++;
    if (jawsNetIdIdent[id].size() > 0) {
      nbJawsSynsets++;
      transInJaws = true;
      // If it's a BCS, count it
      if (BCS.find(id) != BCS.end()) {
        BCSJawsCount[BCS.at(id)]++;
      }
    }

    if (vtNetIdIdent[id].size() > 0) {
      if (nbTermsInSynset > 0) {
        totalPercentageTermsOkInSynset += 1.0 * nbTermsOkInSynset / nbTermsInSynset;
        if (nbPolyTermsInSynset > 0) {
          totalPercentagePolyTermsOkInSynset += 1.0 * nbPolyTermsOkInSynset / nbPolyTermsInSynset;
        }
        if (nbBcsTermsInSynset > 0) {
          totalPercentageBcsTermsOkInSynset += 1.0 * nbBcsTermsOkInSynset / nbBcsTermsInSynset;
        }
      }
    }

    // count terms in GT
    if (vtNetIdIdent[id].size() > 0) {
      nbGtSynsets++;
      for (std::string gtTerm : vtNetIdIdent[id]) {
        // we'll want to display what the GT said.
        transInGt = true;

        // MWE expressions don't count for now
        if (gtTerm.find("_") != std::string::npos) { continue; }

        nbTermsInGt++;
        if (transInJaws) {
          nbTermsInGtAndAJawsSynset++;
        }
        // we're assuming the term is polysemous if it comes from a polysemous synset.
        // a synset is polysemous is one of its terms comes from more than one synset.
        if (polyIdsList.find(id) != polyIdsList.end()) {
          nbPolyTermsInGt++;
          if (transInJaws) {
            nbPolyTermsInGtAndAJawsSynset++;
          }
        }
        if (BCS.find(id) != BCS.end()) {
          nbBcsTermsInGt++;
          if (transInJaws) {
            nbBcsTermsInGtAndAJawsSynset++;
          }
        }
      }
    }

    bool printSynset = false;
    if (gold && goldIds.find(id) != goldIds.end()) {
      printSynset = true;
    } else if (!gold && (transInGt || transInJaws)) {
      printSynset = true;
    }

    if (printSynset) {

      /* count accordance and disagreements
      ----------------------------------------------------*/
      std::set<std::string> agree;
      std::set<std::string> type1;
      std::set<std::string> type2;
      for (auto itJawsTerm : wne.frenchSynset) {
        std::string jawsTerm = itJawsTerm.first;
        // Jaws and GT agree
        if (vtNet[jawsTerm].find(id) != vtNet[jawsTerm].end()) {
          agree.insert(jawsTerm);
        // translations in Jaws not in GT
        } else {
          type2.insert(jawsTerm);
          totalType2++;
        }
      }
      // translations in GT not in Jaws
      for (std::string gtTerm : vtNetIdIdent[id]) {
        if (jawsNetIdIdent[id].find(gtTerm) == jawsNetIdIdent[id].end()) {
          type1.insert(gtTerm);
        }
      }

      /* print an abstract about the synset
      ----------------------------------------------------*/
      out << "\n*****************" << std::endl;
      out << "In the synset " << id << " :" << std::endl;
      out << "Definition : " << wne.def << std::endl;

      // Jaws and GT agree
      if (agree.size() > 0) {
        out << "\n--- Jaws and Gt agree on :" << std::endl;
        for (std::string sameTerm : agree) {
          out << "\"" << sameTerm << "\"";
          if(gold) {
            out << " ("
                      << goldValue[make_pair(id, sameTerm)]
                      << " in gold)";
          }
          out << std::endl;
          for (WORDNET::TranslationInfos itTransInfos : wne.frenchSynset[sameTerm]) {
            out << " - from " << itTransInfos.original
                      << ", with " << itTransInfos.processed
                      << " module, score : " << itTransInfos.score << std::endl;
          }
        }
      }

      // a term in this synset in GT not in Jaws
      if (type1.size() > 0) {
        out << "\n--- Error type 1 : Jaws missed a " << pos << "." << std::endl;
        for (std::string gtTerm : type1) {
          out << "\"" << gtTerm << "\"";
          if(gold) {
            out << " ("
                      << goldValue[make_pair(id, gtTerm)]
                      << " in gold)";
          }
          out << std::endl;
        }
      }

      // a term in this synset in Jaws not in GT
      if (type2.size() > 0) {
        out << "\n--- Error type 2 : this " << pos << " is not in GT." << std::endl;
        for (std::string jawsTerm : type2) {
          out << "\"" << jawsTerm << "\"";
          if(gold) {
            out << " ("
                      << goldValue[make_pair(id, jawsTerm)]
                      << " in gold)";
          }
          out << std::endl;
          for (WORDNET::TranslationInfos itTransInfos : wne.frenchSynset[jawsTerm]) {
            out << " - from " << itTransInfos.original
                      << ", with " << itTransInfos.processed
                      << " module, score : " << itTransInfos.score << std::endl;
          }
        }
      }

    }

    id = std::string();
    wne = WORDNET::WordNetEntry();
  }

}


void JawsHandler::on_end_document() {

  // print an abstract about the evaluation
  std::string terms;
  if (pos == "noun") {
    terms = "Nouns";
  } else if (pos == "verb") {
    terms = "Verbs";
  } else if (pos == "adj") {
    terms = "Adjs";
  } else if (pos == "adv") {
    terms = "Advs";
  }

  float allPrecision = (float)nbTermsOk / (float)nbTermsInJaws;
  float allPseudoPrec = (float)nbTermsOk / (float)nbInJawsSynsetInGt;
  float averagePseudoPrec = totalPercentageTermsOkInSynset / nbInJawsSynsetInGt;
  float allRecGt = (float)nbTermsOk / (float)nbTermsInGtAndAJawsSynset;
  float allF1 = 2*(allPseudoPrec * allRecGt) / (allPseudoPrec + allRecGt);

  float polyPrecision = (float)nbPolyTermsOk / (float)nbPolyTermsInJaws;
  float polyPseudoPrec = (float)nbPolyTermsOk / (float)nbPolyInJawsSynsetInGt;
  float averagePolyPseudoPrec = (float)totalPercentagePolyTermsOkInSynset / (float)nbPolyInJawsSynsetInGt;
  float polyRecGt = (float)nbPolyTermsOk / (float)nbPolyTermsInGtAndAJawsSynset;
  float polycoverWN = (float)nbPolyTermsInJaws / (float)nbPolyOriginals;
  float polyF1 = 2*(polyPseudoPrec * polyRecGt) / (polyPseudoPrec + polyRecGt);

  float bcsPrecision = (float)nbBcsTermsOk / (float)nbBcsTermsInJaws;
  float bcsPseudoPrec = (float)nbBcsTermsOk / (float)nbBcsInJawsSynsetInGt;
  float averageBcsPseudoPrec = (float)totalPercentageBcsTermsOkInSynset / (float)nbBcsInJawsSynsetInGt;
  float bcsRecGt = (float)nbBcsTermsOk / (float)nbBcsTermsInGtAndAJawsSynset;
  float bcscoverWN = (float)nbBcsTermsInJaws / (float)nbBcsOriginals;
  float bcsF1 = 2*(bcsPseudoPrec * bcsRecGt) / (bcsPseudoPrec + bcsRecGt);

  float coverageWN = (float)nbTermsInJaws / (float)nbOriginals;
  float recSynsetsGt = (float)nbJawsSynsets / (float)nbGtSynsets;
  float recallSynsets = (float)nbJawsSynsets / (float)nbSynsets;

  out.setf(std::ios::fixed, std::ios::floatfield);
  out.precision(2);
  std::cout.setf(std::ios::fixed, std::ios::floatfield);
  std::cout.precision(2);

  out << "------------------------------------" << std::endl;

  out << "\t\t*** All " + terms + " ***" << std::endl;
  out << "nb" + terms + "InJaws :\t\t\t" << nbTermsInJaws << std::endl;
  out << "Denominator for precision :\t" << nbInJawsSynsetInGt << std::endl;
  out << "Denominator for recall :\t" << nbTermsInGtAndAJawsSynset << std::endl;
  out << "nb" + terms + "InGt :\t\t\t" << nbTermsInGt << std::endl;
  out << "nb" + terms + "InJawsAgreeWithGt :\t" << nbTermsOk << std::endl;
  out << "Missing " << terms << " in GT:\t\t" << totalType2 << std::endl;
  out << "Precision :\t\t\t" << allPrecision*100 << "%" << std::endl;
  out << "Average pseudo precision :\t" << averagePseudoPrec*100 << "%"  << std::endl;
  out << "Pseudo precision :\t\t" << allPseudoPrec*100 << "%"  << std::endl;
  out << "Recall / GT :\t\t\t" << allRecGt*100 << "%" << std::endl;
  out << "F1-score :\t\t\t" << allF1*100 << "%" << std::endl;
  out << "Coverage / WN :\t\t\t" << coverageWN*100 << "%" << std::endl;

  out << "\t\t*** Polysemous ***" << std::endl;
  out << "nb" + terms + "InJaws :\t\t\t" << nbPolyTermsInJaws << std::endl;
  out << "Denomiator for precision :\t" << nbPolyInJawsSynsetInGt << std::endl;
  out << "Denominator for recall :\t" << nbPolyTermsInGtAndAJawsSynset << std::endl;
  out << "nb" + terms + "InGt :\t\t\t" << nbPolyTermsInGt << std::endl;
  out << "nb" + terms + "InJawsAgreeWithGt :\t" << nbPolyTermsOk << std::endl;
  out << "Precision :\t\t\t" << polyPrecision*100 << "%" << std::endl;
  out << "Average pseudo precision :\t" << averagePolyPseudoPrec*100 << "%"  << std::endl;
  out << "Pseudo precision :\t\t" << polyPseudoPrec*100 << "%" << std::endl;
  out << "Recall / GT :\t\t\t" << polyRecGt*100 << "%" << std::endl;
  out << "F1-score :\t\t\t" << polyF1*100 << "%" << std::endl;
  out << "Coverage / WN :\t\t\t" << polycoverWN*100 << "%" << std::endl;

  out << "\t\t*** BCS ***" << std::endl;
  out << "nb" + terms + "InJaws :\t\t\t" << nbBcsTermsInJaws << std::endl;
  out << "Denominator for precision :\t" << nbBcsInJawsSynsetInGt << std::endl;
  out << "Denominator for recall :\t" << nbBcsTermsInGtAndAJawsSynset << std::endl;
  out << "nb" + terms + "InGt :\t\t\t" << nbBcsTermsInGt << std::endl;
  out << "nb" + terms + "InJawsAgreeWithGt :\t" << nbBcsTermsOk << std::endl;
  out << "Precision :\t\t\t" << bcsPrecision*100 << "%" << std::endl;
  out << "Average pseudo precision :\t" << averageBcsPseudoPrec*100 << "%"  << std::endl;
  out << "Pseudo precision :\t\t" << bcsPseudoPrec*100 << "%" << std::endl;
  out << "Recall / GT :\t\t\t" << bcsRecGt*100 << "%" << std::endl;
  out << "F1-score :\t\t\t" << bcsF1*100 << "%" << std::endl;
  out << "Coverage / WN :\t\t\t" << bcscoverWN*100 << "%" << std::endl;


  out << "---" << std::endl;
  out << "nbOriginals : " << nbOriginals
       << ", polysemous : " << nbPolyOriginals << std::endl;
  out << "nbSynsets in WN : " << nbSynsets
       << ", in Jaws : " << nbJawsSynsets
       << ", in Gt : " << nbGtSynsets << std::endl;
  out << "BCS:\t\t\t\t" << BCSJawsCount[1] << " " << BCSJawsCount[2] << " " << BCSJawsCount[3] << std::endl;
  out << "BCS(%):\t\t\t\t" << 100.0*BCSJawsCount[1]/BCSCount.at(1) << "% " << 100.0*BCSJawsCount[2]/BCSCount.at(2) << "% " << 100.0*BCSJawsCount[3]/BCSCount.at(3) << "%" << std::endl;
  out << "Coverage synsets / GT :\t\t" << recSynsetsGt*100 << "%" << std::endl;
  out << "Coverage synsets / WN :\t\t" << recallSynsets*100 << "%" << std::endl;

  std::set<std::ostream*> logs{&std::cout, &out};
  for(std::ostream* logTo: logs) {
    *logTo << "           All " << std::setw(5) << terms << "           Polysemous              BCS" << std::endl;

    *logTo << std::setw(5) << terms << ":   ";
    *logTo << std::setw(6) << nbTermsInJaws     << " - " << std::setw(5) << coverageWN*100  << "%" << " - " << std::setw(4) << nbInJawsSynsetInGt << "     ";
    *logTo << std::setw(6) << nbPolyTermsInJaws << " - " << std::setw(5) << polycoverWN*100 << "%" << " - " << std::setw(4) << nbPolyInJawsSynsetInGt << "     ";
    *logTo << std::setw(6) << nbBcsTermsInJaws  << " - " << std::setw(5) << bcscoverWN*100  << "%" << " - " << std::setw(4) << nbBcsInJawsSynsetInGt << std::endl;

    *logTo << "P/R/F1:  ";
    *logTo << std::setw(5) << allPseudoPrec * 100  << "% / " << std::setw(5) << allRecGt * 100  << "% / " << std::setw(5) << allF1 * 100  << "%     ";
    *logTo << std::setw(5) << polyPseudoPrec * 100 << "% / " << std::setw(5) << polyRecGt * 100 << "% / " << std::setw(5) << polyF1 * 100 << "%     ";
    *logTo << std::setw(5) << bcsPseudoPrec * 100  << "% / " << std::setw(5) << bcsRecGt * 100  << "% / " << std::setw(5) << bcsF1 * 100  << "%" << std::endl;
  }
/* Used to put results in LaTeX */
  std::cout
    << allPseudoPrec * 100  << "& " << allRecGt * 100  << "& " << nbTermsInJaws     << "&     "
    << polyPseudoPrec * 100 << "& " << polyRecGt * 100 << "& " << nbPolyTermsInJaws << "&     "
    << bcsPseudoPrec * 100  << "& " << bcsRecGt * 100  << "& " << nbBcsTermsInJaws  << "&" << std::endl;
}
