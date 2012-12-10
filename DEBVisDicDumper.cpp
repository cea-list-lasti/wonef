#include "DEBVisDicDumper.hpp"
#include "BCSBaseHandler.hpp"
#include "tools.h"
#include "Paths.hpp"

#include <libxml++/libxml++.h>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>


void DEBVisDicDumperModule::dump(WORDNET::WordNet& wn) {
  std::cerr << "Writing in " << datafile << std::endl;

  std::string spos = WORDNET::string_of_POS[WORDNET::pos];

  std::map<string,int> bcsbase;
  std::map<int,int> BCSCount;
  BcsbaseHandler bcsbaseHandler(bcsbase, BCSCount, spos);
  bcsbaseHandler.parse_file(BCSFILE);

  xmlpp::Document document;
  document.set_internal_subset("WN", "", "debvisdic-strict.dtd");

  xmlpp::Element* wnElem = document.create_root_node("WN", "", "");
  wnElem->set_attribute("pos", spos, "");

  for (const auto& itwn : wn) {
    std::string synsetId = itwn.first;
    WORDNET::WordNetEntry wne = itwn.second;

    std::string pos = WORDNET::BalkaNet_string_of_POS[wne.pos];

    xmlpp::Element* synsetElem = wnElem->add_child("SYNSET");

    /* links between synsets */
    for (const auto& it: wne.ilr) {
      std::string relation_name = it.first;
      if (WORDNET::child_relations.count(relation_name) == 1) {
        continue;
      }

      for (const auto& ilrSynsetId: it.second) {
        xmlpp::Element* ilrElem = synsetElem->add_child("ILR");
        ilrElem->set_child_text(ilrSynsetId);
        ilrElem->set_attribute("type", relation_name);
      }
    }

    synsetElem->add_child("ID")->set_child_text("eng-30-" + synsetId + "-" + pos);

    xmlpp::Element* synonymElem = synsetElem->add_child("SYNONYM");
    if (wne.frenchSynset.empty()) {
      synonymElem->add_child("LITERAL")->set_child_text("_EMPTY_");
    } else {
      for (const auto& itwne : wne.frenchSynset) {
        std::string escaped_translation = escape_xml(itwne.first);
        synonymElem->add_child("LITERAL")->set_child_text(escaped_translation);
      }
    }

    synsetElem->add_child("DEF")->set_child_text(boost::trim_copy(wne.def));
    for(auto usage: wne.usages) {
      synsetElem->add_child("USAGE")->set_child_text(usage);
    }

    if (bcsbase.find(synsetId) != bcsbase.end()) {
      synsetElem->add_child("BCS")->set_child_text(boost::lexical_cast<string>(bcsbase[synsetId]));
    }

    synsetElem->add_child("POS")->set_child_text(pos);
  }

  document.write_to_file_formatted(datafile, "UTF-8");

}



