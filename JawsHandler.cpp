#include "JawsHandler.hpp"
#include "Tools.hpp"

#include <iostream>
#include <iomanip>
#include <boost/lexical_cast.hpp>
#include <math.h>
#include <unicode/unistr.h>
#include <unicode/stringpiece.h>

JawsHandler::JawsHandler(std::set<std::string>& _polyLitList,
                         std::set<std::string>& _polyIdsList,
                         std::map<std::string, std::set<std::string> >& _vtNet,
                         std::map<std::string, std::set<std::string> >& _vtNetIdIdent,
                         std::map<std::pair<std::string, std::string>, int>& _goldValue,
                         bool _gold, std::string _pos,
                         const std::map<std::string, int>& _BCS,
                         const std::map<int, int>& _BCSCount) :
                         nbSynsets(0), nbJawsSynsets(0), nbGtSynsets(0),
                         nbOriginals(0), nbPolyOriginals(0),
                         nbTermsInJaws(0), nbPolyTermsInJaws(0),
                         nbTermsInGt(0), nbPolyTermsInGt(0),
                         nbTermsInGtAndAJawsSynset(0), nbPolyTermsInGtAndAJawsSynset(0),
                         nbTermsOk(0), nbPolyTermsOk(0),
                         nbInJawsSynsetInGt(0), nbPolyInJawsSynsetInGt(0),
                         totalPercentageTermsOkInSynset(0.0f), totalPercentagePolyTermsOkInSynset(0.0f),
                         polyLitList(_polyLitList), polyIdsList(_polyIdsList),
                         vtNet(_vtNet), vtNetIdIdent(_vtNetIdIdent),
                         goldValue(_goldValue), gold (_gold), pos(_pos), BCS(_BCS), BCSCount(_BCSCount) {

  candidates = std::map<std::string, std::set<std::string > >();

  if(gold) {
    for (auto itGold : goldValue) {
      goldIds.insert((itGold.first).first);
    }
  }

  xercesc::XMLTransService* const theService = xercesc::XMLPlatformUtils::fgTransService;
  xercesc::XMLTransService::Codes theCode;
  theTranscoder = theService->makeNewTranscoderFor("utf-8", theCode, 8192);

  BCSJawsCount = { {1, 0}, {2, 0}, {3, 0} };
}

JawsHandler::~JawsHandler() {
  delete theTranscoder;
}


void JawsHandler::startElement(const XMLCh *const /*uri*/,
                               const XMLCh *const /*localname*/,
                               const XMLCh *const qname,
                               const xercesc::Attributes & attrs){

  if(_transcode(qname, theTranscoder) == "SYNSET") {
    id = getAttrValue(attrs, "id", theTranscoder);

    nbTermsOkInSynset = 0;
    nbPolyTermsOkInSynset = 0;
    nbTermsInSynset = 0;
    nbPolyTermsInSynset = 0;


  } else if(_transcode(qname, theTranscoder) == "CANDIDATES") {
    original = getAttrValue(attrs, "original", theTranscoder);
    wne.frenchCandidates[original] = WORDNET::TgtCandidates();

  } else if (_transcode(qname, theTranscoder) == "INSTANCES") {
    translation = getAttrValue(attrs, "translation", theTranscoder);
    wne.frenchSynset[translation] = std::set<WORDNET::TranslationInfos>();

  } else if(_transcode(qname, theTranscoder) == "INSTANCE") {
    WORDNET::TranslationInfos transInfos;
    transInfos.original = getAttrValue(attrs, "original", theTranscoder);
    transInfos.processed = getAttrValue(attrs, "processed", theTranscoder);
    string score = getAttrValue(attrs, "score", theTranscoder);
    transInfos.score = boost::lexical_cast<float>(score);
    wne.frenchSynset[translation].insert(transInfos);
  }

}


void JawsHandler::characters(const XMLCh *const chars,
                             const XMLSize_t /*length*/){
  tmpString = _transcode(chars, theTranscoder);
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


void JawsHandler::endElement(const XMLCh *const /*uri*/,
                             const XMLCh *const /*localname*/,
                             const XMLCh *const qname){

   if(_transcode(qname, theTranscoder) == "ORIGINALDEF") {
    wne.def = tmpString;

   } else if(_transcode(qname, theTranscoder) == "CANDIDATES") {
    nbOriginals++;
    if (polyLitList.find(original) != polyLitList.end()) {
      nbPolyOriginals++;
    }
    original = string();

  } else if (_transcode(qname, theTranscoder) == "INSTANCES") {
    if (translation.find("_") != std::string::npos) { return; }

    // check if the translation comes from polysemous source terms
    bool polysemous = false;
    for (WORDNET::TranslationInfos itOrig : wne.frenchSynset[translation]) {
      if (polyLitList.find(itOrig.original) != polyLitList.end()) {
        polysemous = true;
      }
    }
    // count terms in Jaws
    nbTermsInJaws++;
    nbTermsInSynset++;
    if (polysemous) {
      nbPolyTermsInJaws++;
      nbPolyTermsInSynset++;
    }

    // count terms in Jaws which are in synsets translated in GT
    if (vtNetIdIdent[id].size() > 0) {
      nbInJawsSynsetInGt++;
      if (polysemous) {
        nbPolyInJawsSynsetInGt++;
      }

      // count terms in the same synset in JAWS and GT
      if (gtHasTranslation(vtNetIdIdent[id], translation)) {
        nbTermsOk++;
        nbTermsOkInSynset++;
        if (polysemous == true) {
          nbPolyTermsOk++;
          nbPolyTermsOkInSynset++;
        }
      }
    }
    jawsNet[translation].insert(id);
    jawsNetIdIdent[id].insert(translation);
    translation = string();

  } else if(_transcode(qname, theTranscoder) == "SYNSET") {
    bool transInGt = false;
    bool transInJaws = false;
    nbSynsets++;
    if (jawsNetIdIdent[id].size() > 0) {
      nbJawsSynsets++;
      transInJaws = true;
      // If it's a BCS, count it
      if(BCS.find(id) != BCS.end()) {
        BCSJawsCount[BCS.at(id)]++;
      }
    }

    if (vtNetIdIdent[id].size() > 0) {
      if (nbTermsInSynset > 0) {
        totalPercentageTermsOkInSynset += 1.0 * nbTermsOkInSynset / nbTermsInSynset;
        if (nbPolyTermsInSynset > 0) {
          totalPercentagePolyTermsOkInSynset += 1.0 * nbPolyTermsOkInSynset / nbPolyTermsInSynset;
        }
      }
    }

    // count terms in GT
    if (vtNetIdIdent[id].size() > 0) {
      nbGtSynsets++;
      for (std::string gtTerm : vtNetIdIdent[id]) {
        // MWE expressions don't count for now
        if (gtTerm.find("_") != std::string::npos) { continue; }

        nbTermsInGt++;
        if (transInJaws) {
          nbTermsInGtAndAJawsSynset++;
        }
        transInGt = true;
        if (polyIdsList.find(id) != polyIdsList.end()) {
          nbPolyTermsInGt++;
          if (transInJaws) {
            nbPolyTermsInGtAndAJawsSynset++;
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
      std::cout << "\n*****************" << endl;
      std::cout << "In the synset " << id << " :" << endl;
      std::cout << "Definition : " << wne.def << endl;

      // Jaws and GT agree
      if (agree.size() > 0) {
        std::cout << "\n--- Jaws and Gt agree on :" << endl;
        for (std::string sameTerm : agree) {
          std::cout << "\"" << sameTerm << "\"";
          if(gold) {
            std::cout << " ("
                      << goldValue[make_pair(id, sameTerm)]
                      << " in gold)";
          }
          std::cout << endl;
          for (WORDNET::TranslationInfos itTransInfos : wne.frenchSynset[sameTerm]) {
            std::cout << " - from " << itTransInfos.original
                      << ", with " << itTransInfos.processed
                      << " module, score : " << itTransInfos.score << endl;
          }
        }
      }

      // a term in this synset in GT not in Jaws
      if (type1.size() > 0) {
        std::cout << "\n--- Error type 1 : Jaws missed a " << pos << "." << endl;
        for (std::string gtTerm : type1) {
          std::cout << "\"" << gtTerm << "\"";
          if(gold) {
            std::cout << " ("
                      << goldValue[make_pair(id, gtTerm)]
                      << " in gold)";
          }
          std::cout << endl;
        }
      }

      // a term in this synset in Jaws not in GT
      if (type2.size() > 0) {
        std::cout << "\n--- Error type 2 : this " << pos << " is not in GT." << endl;
        for (std::string jawsTerm : type2) {
          std::cout << "\"" << jawsTerm << "\"";
          if(gold) {
            std::cout << " ("
                      << goldValue[make_pair(id, jawsTerm)]
                      << " in gold)";
          }
          std::cout << endl;
          for (WORDNET::TranslationInfos itTransInfos : wne.frenchSynset[jawsTerm]) {
            std::cout << " - from " << itTransInfos.original
                      << ", with " << itTransInfos.processed
                      << " module, score : " << itTransInfos.score << endl;
          }
        }
      }

    }

    id = string();
    wne = WORDNET::WordNetEntry();
  }

}


void JawsHandler::endDocument() {

  // print an abstract about the evaluation
  string terms;
  if (pos == "noun") {
    terms = "Nouns";
  } else if (pos == "verb") {
    terms = "Verbs";
  } else if (pos == "adj") {
    terms = "Adjs";
  }

  float allPrecision = (float)nbTermsOk / (float)nbTermsInJaws;
  float allPseudoPrec = (float)nbTermsOk / (float)nbInJawsSynsetInGt;
  float averagePseudoPrec = totalPercentageTermsOkInSynset / nbInJawsSynsetInGt;
  float polyPrecision = (float)nbPolyTermsOk / (float)nbPolyTermsInJaws;
  float polyPseudoPrec = (float)nbPolyTermsOk / (float)nbPolyInJawsSynsetInGt;
  float averagePolyPseudoPrec = totalPercentagePolyTermsOkInSynset / nbPolyInJawsSynsetInGt;
  float allRecGt = (float)nbTermsOk / (float)nbTermsInGtAndAJawsSynset;
  float polyRecGt = (float)nbPolyTermsOk / (float)nbPolyTermsInGtAndAJawsSynset;
  float allF1 = 2*(allPseudoPrec * allRecGt) / (allPseudoPrec + allRecGt);
  float polyF1 = 2*(polyPseudoPrec * polyRecGt) / (polyPseudoPrec + polyRecGt);
  float coverageWN = (float)nbTermsInJaws / (float)nbOriginals;
  float polycoverWN = (float)nbPolyTermsInJaws / (float)nbPolyOriginals;
  float recSynsetsGt = (float)nbJawsSynsets / (float)nbGtSynsets;
  float recallSynsets = (float)nbJawsSynsets / (float)nbSynsets;

  cout.setf(ios::fixed, ios::floatfield);
  cout.precision(2);

  cout << "------------------------------------" << endl;
  cout << "\t\t*** All " + terms + " ***" << endl;
  cout << "nb" + terms + "InJaws :\t\t\t" << nbTermsInJaws << endl;
  cout << "In synsets known by GT :\t" << nbInJawsSynsetInGt << endl;
  cout << "nb" + terms + "InGt :\t\t\t" << nbTermsInGt << endl;
  cout << "nb" + terms + "InJawsAgreeWithGt :\t" << nbTermsOk << endl;

  cout << "Precision :\t\t\t" << allPrecision*100 << "%" << endl;
  cout << "Average pseudo precision :\t" << averagePseudoPrec*100 << "%"  << endl;
  cout << "Pseudo precision :\t\t" << allPseudoPrec*100 << "%"  << endl;
  cout << "Recall / GT :\t\t\t" << allRecGt*100 << "%" << endl;
  cout << "F1-score :\t\t\t" << allF1*100 << "%" << endl;
  cout << "Coverage / WN :\t\t\t" << coverageWN*100 << "%" << endl;

  cout << "\t\t*** Polysemous ***" << endl;
  cout << "nb" + terms + "InJaws :\t\t\t" << nbPolyTermsInJaws << endl;
  cout << "In synsets known by GT :\t" << nbPolyInJawsSynsetInGt << endl;
  cout << "nb" + terms + "InGt :\t\t\t" << nbPolyTermsInGt << endl;
  cout << "nb" + terms + "InJawsAgreeWithGt :\t" << nbPolyTermsOk << endl;

  cout << "Precision :\t\t\t" << polyPrecision*100 << "%" << endl;
  cout << "Average pseudo precision :\t" << averagePolyPseudoPrec*100 << "%"  << endl;
  cout << "Pseudo precision :\t\t" << polyPseudoPrec*100 << "%" << endl;
  cout << "Recall / GT :\t\t\t" << polyRecGt*100 << "%" << endl;
  cout << "F1-score :\t\t\t" << polyF1*100 << "%" << endl;
  cout << "Coverage / WN :\t\t\t" << polycoverWN*100 << "%" << endl;
  cout << "---" << endl;
  cout << "nbOriginals : " << nbOriginals
       << ", polysemous : " << nbPolyOriginals << endl;
  cout << "nbSynsets in WN : " << nbSynsets
       << ", in Jaws : " << nbJawsSynsets
       << ", in Gt : " << nbGtSynsets << endl;
  cout << "BCS:\t\t\t\t" << BCSJawsCount[1] << " " << BCSJawsCount[2] << " " << BCSJawsCount[3] << endl;
  cout << "BCS(%):\t\t\t\t" << 100.0*BCSJawsCount[1]/BCSCount.at(1) << "% " << 100.0*BCSJawsCount[2]/BCSCount.at(2) << "% " << 100.0*BCSJawsCount[3]/BCSCount.at(3) << "%" << endl;
  cout << "Coverage synsets / GT :\t\t" << recSynsetsGt*100 << "%" << endl;
  cout << "Coverage synsets / WN :\t\t" << recallSynsets*100 << "%" << endl;

  cout << "           All " << setw(5) << terms << "            Polysemous" << endl;

  cout << setw(5) << terms << ":  ";
  cout << setw(6) << nbTermsInJaws << " - " << setw(5) << coverageWN*100 << "%" << "     ";
  cout << setw(6) << nbPolyTermsInJaws << " - " << setw(5) << polycoverWN*100 << "%" << endl;

  cout << "P/R:    ";
  cout << setw(5) << allPseudoPrec * 100 << "% / " << setw(5) << allRecGt * 100 << "%" << "     ";
  cout << setw(5) << polyPseudoPrec * 100 << "% / " << setw(5) << polyRecGt * 100 << "%" << endl;


  cout << "BCS(%):         " << 100.0*BCSJawsCount[1]/BCSCount.at(1) << "% " << 100.0*BCSJawsCount[2]/BCSCount.at(2) << "% " << 100.0*BCSJawsCount[3]/BCSCount.at(3) << "%" << endl;
}
