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
                         nbSynsets(0), nbJawsSynsets(0), nbVtSynsets(0),
                         nbOriginals(0), nbPolyOriginals(0),
                         nbTermsInJaws(0), nbPolyTermsInJaws(0),
                         nbTermsInVt(0), nbPolyTermsInVt(0),
                         nbTermsOk(0), nbPolyTermsOk(0),
                         nbInJawsSynsetInVt(0), nbPolyInJawsSynsetInVt(0),
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

    // count terms in Jaws which are in synsets translated in VT
    if (vtNetIdIdent[id].size() > 0) {
      nbInJawsSynsetInVt++;
      if (polysemous) {
        nbPolyInJawsSynsetInVt++;
      }

      // count terms in the same synset in JAWS and VT
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
    bool transInVt = false;
    bool transInJaws = false;
    nbSynsets++;
    if (jawsNetIdIdent[id].size() > 0) {
      nbJawsSynsets++;
      transInJaws = true;
    }

    // count terms in VT
    if (vtNetIdIdent[id].size() > 0) {
      nbVtSynsets++;
      for (std::set<std::string>::iterator itVtTerm = vtNetIdIdent[id].begin();
           itVtTerm != vtNetIdIdent[id].end(); itVtTerm++) {
        nbTermsInVt++;
        transInVt = true;
        if (polyIdsList.find(id) != polyIdsList.end()) {
          nbPolyTermsInVt++;
        }
      }
    }

    bool printSynset = false;
    if (gold && goldIds.find(id) != goldIds.end()) {
      printSynset = true;
    } else if (!gold && (transInVt || transInJaws)) {
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
        // Jaws and VT agree
        if (vtNet[itJawsTerm->first].find(id) != vtNet[itJawsTerm->first].end()) {
          agree.insert(itJawsTerm->first);
        // translations in Jaws not in VT
        } else {
          type2.insert(itJawsTerm->first);
        }
      }
      // translations in VT not in Jaws
      for (std::set<std::string>::iterator itVtTerm = vtNetIdIdent[id].begin();
           itVtTerm != vtNetIdIdent[id].end(); itVtTerm++) {
        if (jawsNetIdIdent[id].find(*itVtTerm) == jawsNetIdIdent[id].end()) {
          type1.insert(*itVtTerm);
        }
      }

      /* print an abstract about the synset
      ----------------------------------------------------*/
      std::cout << "\n*****************" << endl;
      std::cout << "In the synset " << id << " :" << endl;
      std::cout << "Definition : " << wne.def << endl;

      // Jaws and VT agree
      if (agree.size() > 0) {
        std::cout << "\n--- Jaws and Vt agree on :" << endl;
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

      // a term in this synset in VT not in Jaws
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

      // a term in this synset in Jaws not in VT
      if (type2.size() > 0) {
        std::cout << "\n--- Error type 2 : this " << pos << " is not in VT." << endl;
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
  float allPseudoPrec = (float)nbTermsOk / (float)nbInJawsSynsetInVt;
  float polyPrecision = (float)nbPolyTermsOk / (float)nbPolyTermsInJaws;
  float polyPseudoPrec = (float)nbPolyTermsOk / (float)nbPolyInJawsSynsetInVt;
  float allRecVt = (float)nbInJawsSynsetInVt / (float)nbTermsInVt;
  float polyRecVt = (float)nbPolyInJawsSynsetInVt / (float)nbPolyTermsInVt;
  float allF1 = 2*( (float)allPseudoPrec * (float)allRecVt )
                / ( (float)allPseudoPrec + (float)allRecVt );
  float polyF1 = 2*( (float)polyPseudoPrec * (float)polyRecVt )
                / ( (float)polyPseudoPrec + (float)polyRecVt );
  float allRecall = (float)nbTermsInJaws / (float)nbOriginals;
  float polyRecall = (float)nbPolyTermsInJaws / (float)nbPolyOriginals;
  float recSynsetsVt = (float)nbJawsSynsets / (float)nbVtSynsets;
  float recallSynsets = (float)nbJawsSynsets / (float)nbSynsets;

  cout.setf(ios::fixed, ios::floatfield);
  cout.precision(2);

  cout << "------------------------------------" << endl;
  cout << "\t\t*** All " + terms + " ***" << endl;
  cout << "nb" + terms + "InJaws :\t\t\t" << nbTermsInJaws << endl;
  cout << "In synsets known by Vt :\t" << nbInJawsSynsetInVt << endl;
  cout << "nb" + terms + "InVt :\t\t\t" << nbTermsInVt << endl;
  cout << "nb" + terms + "InJawsAgreeWithVt :\t" << nbTermsOk << endl;

  cout << "Precision :\t\t\t" << allPrecision*100 << "%" << endl;
  cout << "Pseudo precision :\t\t" << allPseudoPrec*100 << "%"  << endl;
  cout << "Recall / VT :\t\t\t" << allRecVt*100 << "%" << endl;
  cout << "Recall / WN :\t\t\t" << allRecall*100 << "%" << endl;
  cout << "F1-score :\t\t\t" << allF1*100 << "%" << endl;

  cout << "\t\t*** Polysemous ***" << endl;
  cout << "nb" + terms + "InJaws :\t\t\t" << nbPolyTermsInJaws << endl;
  cout << "In synsets known by Vt :\t" << nbPolyInJawsSynsetInVt << endl;
  cout << "nb" + terms + "InVt :\t\t\t" << nbPolyTermsInVt << endl;
  cout << "nb" + terms + "InJawsAgreeWithVt :\t" << nbPolyTermsOk << endl;

  cout << "Precision :\t\t\t" << polyPrecision*100 << "%" << endl;
  cout << "Pseudo precision :\t\t" << polyPseudoPrec*100 << "%" << endl;
  cout << "Recall / VT :\t\t\t" << polyRecVt*100 << "%" << endl;
  cout << "Recall / WN :\t\t\t" << polyRecall*100 << "%" << endl;
  cout << "F1-score :\t\t\t" << polyF1*100 << "%" << endl;
  cout << "---" << endl;
  cout << "nbOriginals : " << nbOriginals
       << ", polysemous : " << nbPolyOriginals << endl;
  cout << "nbSynsets in WN : " << nbSynsets
       << ", in Jaws : " << nbJawsSynsets
       << ", in Vt : " << nbVtSynsets << endl;
  cout << "Recall synsets / VT :\t\t" << recSynsetsVt*100 << "%" << endl;
  cout << "Recall synsets / WN :\t\t" << recallSynsets*100 << "%" << endl;

}