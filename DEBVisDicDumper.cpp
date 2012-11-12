#include "DEBVisDicDumper.hpp"
#include "tools.h"

#include <libxml++/libxml++.h>
#include <boost/algorithm/string/trim.hpp>

#include <iostream>


void DEBVisDicDumperModule::dump(WORDNET::WordNet& wn) {
  std::cerr << "Writing in " << datafile << std::endl;

  xmlpp::Document document;
  document.set_internal_subset("WN", "", "debvisdic-strict.dtd");

  xmlpp::Element* wnElem = document.create_root_node("WN", "", "");
  wnElem->set_attribute("pos", "somepos", "");

  for (const auto& itwn : wn) {
    std::string synsetId = itwn.first;
    WORDNET::WordNetEntry wne = itwn.second;

    std::map<POS, std::string> string_of_POS = {{POS::Noun, "n"}, {POS::Verb, "v"}, {POS::Adj, "a"}};
    std::string pos = string_of_POS[wne.pos];

    xmlpp::Element* synsetElem = wnElem->add_child("SYNSET");

    /* links between synsets */
    std::multimap<std::string, std::string> ilrs;
    for(const std::string& it : wne.meros)  { ilrs.insert(std::make_pair("meronym", it)); }
    for(const std::string& it : wne.hypos)  { ilrs.insert(std::make_pair("hyponym", it)); }
    for(const std::string& it : wne.hypers) { ilrs.insert(std::make_pair("hyperonym", it)); }
    for(const std::string& it : wne.hypos)  { ilrs.insert(std::make_pair("hyponym", it)); }

    for(const auto& it: ilrs) {
      xmlpp::Element* ilrElem = synsetElem->add_child("ILR");
      ilrElem->set_child_text(it.second);
      ilrElem->set_attribute("type", it.first);
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
    synsetElem->add_child("BCS")->set_child_text("3");
    synsetElem->add_child("POS")->set_child_text(pos);
  }

  document.write_to_file_formatted(datafile);

}



