#include "JawsHandler.hpp"
#include "Tools.hpp"
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <math.h>

JawsHandler::JawsHandler(std::set<std::string>& _polyLitList,
                         std::set<std::string>& _polyIdsList,
                         std::map<std::string, std::set<std::string> >& _vtNet,
                         std::map<std::string, std::set<std::string> >& _vtNetIdIdent,
                         std::map<std::pair<std::string, std::string>, int>& _goldValue,
                         bool _gold, std::string _pos) :
                         nbSynsets(0), nbJawsSynsets(0), nbGtSynsets(0),
                         nbOriginals(0), nbPolyOriginals(0),
                         nbTermsInJaws(0), nbPolyTermsInJaws(0),
                         nbTermsInGt(0), nbPolyTermsInGt(0),
                         nbTermsOk(0), nbPolyTermsOk(0),
                         nbInJawsSynsetInGt(0), nbPolyInJawsSynsetInGt(0),
                         polyLitList(_polyLitList), polyIdsList(_polyIdsList),
                         vtNet(_vtNet), vtNetIdIdent(_vtNetIdIdent),
                         goldValue(_goldValue),gold (_gold), pos(_pos) {

  candidates = std::map<std::string, std::set<std::string > >();

  if(gold) {
    for (std::map<std::pair<std::string, std::string>, int>::iterator
         itGold = goldValue.begin(); itGold != goldValue.end(); itGold++) {
      goldIds.insert((itGold->first).first);
    }
  }

  xercesc::XMLTransService* const theService = xercesc::XMLPlatformUtils::fgTransService;
  xercesc::XMLTransService::Codes theCode;
  theTranscoder = theService->makeNewTranscoderFor("utf-8", theCode, 8192);

}


JawsHandler::~JawsHandler() {
  delete theTranscoder;
}


void JawsHandler::startElement(const XMLCh *const /*uri*/,
                               const XMLCh *const /*localname*/,
                               const XMLCh *const qname,
                               const xercesc::Attributes & attrs){

  if(_transcode(qname, theTranscoder).compare("SYNSET") == 0) {
    id = getAttrValue(attrs, "id", theTranscoder);

  } else if(_transcode(qname, theTranscoder).compare("CANDIDATES") == 0) {
    original = getAttrValue(attrs, "original", theTranscoder);
    wne.frenchCandidates[original] = WORDNET::TgtCandidates();

  } else if (_transcode(qname, theTranscoder).compare("INSTANCES") == 0) {
    translation = getAttrValue(attrs, "translation", theTranscoder);
    wne.frenchSynset[translation] = std::set<WORDNET::TranslationInfos>();

  } else if(_transcode(qname, theTranscoder).compare("INSTANCE") == 0) {
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


void JawsHandler::endElement(const XMLCh *const /*uri*/,
                             const XMLCh *const /*localname*/,
                             const XMLCh *const qname){

   if(_transcode(qname, theTranscoder).compare("ORIGINALDEF") == 0) {
    wne.def = tmpString;

   } else if(_transcode(qname, theTranscoder).compare("CANDIDATES") == 0) {
    nbOriginals++;
    if (polyLitList.find(original) != polyLitList.end()) {
      nbPolyOriginals++;    
    }
    original = string();

  } else if (_transcode(qname, theTranscoder).compare("INSTANCES") == 0) {
    // check if the translation comes from polysemous source terms
    bool polysemous = false;
    for (set<WORDNET::TranslationInfos>::iterator
         itOrig = wne.frenchSynset[translation].begin();
         itOrig != wne.frenchSynset[translation].end();
         itOrig++){
      if (polyLitList.find(itOrig->original) != polyLitList.end()) {
        polysemous = true;
      }
    }
    // count terms in Jaws
    nbTermsInJaws++;
    if (polysemous) {
      nbPolyTermsInJaws++;
    }

    // count terms in Jaws which are in synsets translated in GT
    if (vtNetIdIdent[id].size() > 0) {
      nbInJawsSynsetInGt++;
      if (polysemous) {
        nbPolyInJawsSynsetInGt++;
      }

      // count terms in the same synset in JAWS and GT
      if (vtNetIdIdent[id].find(translation) != vtNetIdIdent[id].end()) {
        nbTermsOk++;
        if (polysemous == true) {
          nbPolyTermsOk++;
        }
      }
    }
    jawsNet[translation].insert(id);
    jawsNetIdIdent[id].insert(translation);
    translation = string();

  } else if(_transcode(qname, theTranscoder).compare("SYNSET") == 0) {
    bool transInGt = false;
    bool transInJaws = false;
    nbSynsets++;
    if (jawsNetIdIdent[id].size() > 0) {
      nbJawsSynsets++;
      transInJaws = true;
    }

    // count terms in GT
    if (vtNetIdIdent[id].size() > 0) {
      nbGtSynsets++;
      for (std::set<std::string>::iterator itGtTerm = vtNetIdIdent[id].begin();
           itGtTerm != vtNetIdIdent[id].end(); itGtTerm++) {
        nbTermsInGt++;
        transInGt = true;
        if (polyIdsList.find(id) != polyIdsList.end()) {
          nbPolyTermsInGt++;
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
      for (std::map<std::string, std::set<WORDNET::TranslationInfos> >::iterator
           itJawsTerm = wne.frenchSynset.begin();
           itJawsTerm != wne.frenchSynset.end();
           itJawsTerm++) {
        // Jaws and GT agree
        if (vtNet[itJawsTerm->first].find(id) != vtNet[itJawsTerm->first].end()) {
          agree.insert(itJawsTerm->first);
        // translations in Jaws not in GT
        } else {
          type2.insert(itJawsTerm->first);
        }
      }
      // translations in GT not in Jaws
      for (std::set<std::string>::iterator itGtTerm = vtNetIdIdent[id].begin();
           itGtTerm != vtNetIdIdent[id].end(); itGtTerm++) {
        if (jawsNetIdIdent[id].find(*itGtTerm) == jawsNetIdIdent[id].end()) {
          type1.insert(*itGtTerm);
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
        for (std::set<std::string>::iterator itAgree = agree.begin();
             itAgree != agree.end(); itAgree++) {
          std::cout << "\"" << *itAgree << "\"";
          if(gold) {
            std::cout << " ("
                      << goldValue[std::pair<std::string, std::string>(id, *itAgree)]
                      << " in gold)";
          }
          std::cout << endl;
          for (std::set<WORDNET::TranslationInfos>::iterator
              itTransInfos = wne.frenchSynset[*itAgree].begin();
              itTransInfos != wne.frenchSynset[*itAgree].end();
              itTransInfos++) {
            std::cout << " - from " << itTransInfos->original
                      << ", with " << itTransInfos->processed
                      << " module, score : " << itTransInfos->score << endl;
          }
        }
      }

      // a term in this synset in GT not in Jaws
      if (type1.size() > 0) {
        std::cout << "\n--- Error type 1 : Jaws missed a " << pos << "." << endl;
        for (std::set<std::string>::iterator itType1 = type1.begin();
             itType1 != type1.end(); itType1++) {
        std::cout << "\"" << *itType1 << "\"";
          if(gold) {
            std::cout << " ("
                      << goldValue[std::pair<std::string, std::string>(id, *itType1)]
                      << " in gold)";
          }
          std::cout << endl;
        }
      }

      // a term in this synset in Jaws not in GT
      if (type2.size() > 0) {
        std::cout << "\n--- Error type 2 : this " << pos << " is not in GT." << endl;
        for (std::set<std::string>::iterator itType2 = type2.begin();
             itType2 != type2.end(); itType2++) {
          std::cout << "\"" << *itType2 << "\"";
          if(gold) {
            std::cout << " ("
                      << goldValue[std::pair<std::string, std::string>(id, *itType2)]
                      << " in gold)";
          }
          std::cout << endl;
          for (std::set<WORDNET::TranslationInfos>::iterator
              itTransInfos = wne.frenchSynset[*itType2].begin();
              itTransInfos != wne.frenchSynset[*itType2].end();
              itTransInfos++) {
            std::cout << " - from " << itTransInfos->original
                      << ", with " << itTransInfos->processed
                      << " module, score : " << itTransInfos->score << endl;
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
  }

  float allPrecision = (float)nbTermsOk / (float)nbTermsInJaws;
  float allPseudoPrec = (float)nbTermsOk / (float)nbInJawsSynsetInGt;
  float polyPrecision = (float)nbPolyTermsOk / (float)nbPolyTermsInJaws;
  float polyPseudoPrec = (float)nbPolyTermsOk / (float)nbPolyInJawsSynsetInGt;
  float allRecGt = (float)nbTermsOk / (float)nbTermsInGt;
  float polyRecGt = (float)nbPolyTermsOk / (float)nbPolyTermsInGt;
  float allF1 = 2*( (float)allPseudoPrec * (float)allRecGt )
                / ( (float)allPseudoPrec + (float)allRecGt );
  float polyF1 = 2*( (float)polyPseudoPrec * (float)polyRecGt )
                / ( (float)polyPseudoPrec + (float)polyRecGt );
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
  cout << "Coverage synsets / GT :\t\t" << recSynsetsGt*100 << "%" << endl;
  cout << "Coverage synsets / WN :\t\t" << recallSynsets*100 << "%" << endl;

}