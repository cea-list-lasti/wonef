#ifndef JAWSHANDLER_HPP
#define JAWSHANDLER_HPP

#include "WordNet.hpp"
#include <libxml++/libxml++.h>
#include <fstream>
#include <string>
#include <sstream>
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

  void on_start_element(const std::string& name, const xmlpp::SaxParser::AttributeList& properties);
  void on_characters(const std::string& characters);
  void on_end_element(const std::string &name);
  void on_end_document();

  std::ofstream& out;

  uint nbSynsets;
  uint nbJawsSynsets;
  uint nbGtSynsets;

  uint nbOriginals;
  uint nbPolyOriginals;
  uint nbBcsOriginals;

  uint nbTermsInJaws;
  uint nbPolyTermsInJaws;
  uint nbBcsTermsInJaws;

  uint nbTermsInGt;
  uint nbPolyTermsInGt;
  uint nbBcsTermsInGt;

  // useful for recall: terms in GT, but only in a non-empty JAWS synset
  uint nbTermsInGtAndAJawsSynset;
  uint nbPolyTermsInGtAndAJawsSynset;
  uint nbBcsTermsInGtAndAJawsSynset;

  uint nbTermsOk;
  uint nbPolyTermsOk;
  uint nbBcsTermsOk;

  uint nbInJawsSynsetInGt;
  uint nbPolyInJawsSynsetInGt;
  uint nbBcsInJawsSynsetInGt;

  /* per-synset average precision */
  uint nbTermsOkInSynset;
  uint nbPolyTermsOkInSynset;
  uint nbBcsTermsOkInSynset;

  uint nbTermsInSynset;
  uint nbPolyTermsInSynset;
  uint nbBcsTermsInSynset;

  float totalPercentageTermsOkInSynset;
  float totalPercentagePolyTermsOkInSynset;
  float totalPercentageBcsTermsOkInSynset;

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
  std::stringstream tmpString;
  std::string pos;

  /* Basic Concept Sets as defined by balkanet */
  const std::map<std::string, int>& BCS;
  const std::map<int, int>& BCSCount;
  std::map<int, int> BCSJawsCount;

  int totalType2;

private:
  std::string get_attr(const xmlpp::SaxParser::AttributeList& attrs, std::string name);

};

#endif
