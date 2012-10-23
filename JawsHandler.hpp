#ifndef JAWSHANDLER_HPP
#define JAWSHANDLER_HPP

#include "WordNet.hpp"
#include <libxml++/libxml++.h>
#include <fstream>
#include <string>
#include <set>
#include <map>

class JawsHandler : public xmlpp::SaxParser {

public :
  JawsHandler(
      std::ofstream& _out,
      std::set<std::string>& _polyLitList,
      std::set<std::string>& _polyIdsList,
      std::map<std::string, std::set<std::string> >& _vtNet,
      std::map<std::string, std::set<std::string> >& _vtNetIdIdent,
      std::map<std::pair<std::string, std::string>, int>& _goldValue,
      bool _gold, std::string _pos,
      const std::map<std::string, int>& BCS, const std::map<int, int>& BCSCount);

  virtual ~JawsHandler() {}

  void on_start_element(const std::string& name, const xmlpp::SaxParser::AttributeList& properties) override;
  void on_characters(const std::string& characters) override;
  void on_end_element(const std::string &name) override;
  void on_end_document() override;

  std::ofstream& out;

  uint nbSynsets;
  uint nbJawsSynsets;
  uint nbGtSynsets;
  uint nbOriginals;
  uint nbPolyOriginals;
  uint nbTermsInJaws;
  uint nbPolyTermsInJaws;
  uint nbTermsInGt;
  uint nbPolyTermsInGt;
  // useful for recall: terms in GT, but only in a non-empty JAWS synset
  uint nbTermsInGtAndAJawsSynset;
  uint nbPolyTermsInGtAndAJawsSynset;
  uint nbTermsOk;
  uint nbPolyTermsOk;
  uint nbInJawsSynsetInGt;
  uint nbPolyInJawsSynsetInGt;

  /* per-synset average precision */
  uint nbTermsOkInSynset;
  uint nbPolyTermsOkInSynset;
  uint nbTermsInSynset;
  uint nbPolyTermsInSynset;
  float totalPercentageTermsOkInSynset;
  float totalPercentagePolyTermsOkInSynset;

protected :

  std::set<std::string> polyLitList;
  std::set<std::string> polyIdsList;
  // map<englishWord, set<frenchWords> >
  std::map<std::string, std::set<std::string> > candidates;
  // map<frenchWord, set<ids> >
  std::map<std::string, std::set<std::string> > vtNet;
  std::map<std::string, std::set<std::string> > jawsNet;
  // map<id, set<frenchWords> >
  std::map<std::string, std::set<std::string> > vtNetIdIdent;
  std::map<std::string, std::set<std::string> > jawsNetIdIdent;
  // map<pair<id, frenchWord>, value>
  std::map<std::pair<std::string, std::string>, int> goldValue;
  std::set<std::string> goldIds;
  bool gold;
  WORDNET::WordNetEntry wne;
  std::string id;
  std::string original;
  std::string translation;
  std::string tmpString;
  std::string pos;

  /* Basic Concept Sets as defined by balkanet */
  const std::map<std::string, int>& BCS;
  const std::map<int, int>& BCSCount;
  std::map<int, int> BCSJawsCount;

};

#endif
