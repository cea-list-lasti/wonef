#include "DEBVisDicDumper.hpp"
#include "BCSBaseHandler.hpp"
#include "tools.h"
#include "Paths.hpp"

#include <libxml++/libxml++.h>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>


void DEBVisDicDumperModule::dump(WORDNET::WordNet& wn) {
  std::string spos = WORDNET::string_of_POS[WORDNET::pos];

  std::map<std::string,int> bcsbase;
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
      /* Too many child relations: DEBVisDic avoids them */
      if (WORDNET::child_relations.count(relation_name) == 1) {
        continue;
      }

      /* Relation to ilrSynsetId with type relation_name */
      for (const auto& ilrSynsetId: it.second) {
        xmlpp::Element* ilrElem = synsetElem->add_child("ILR");
        ilrElem->set_child_text(ilrSynsetId);
        ilrElem->set_attribute("type", relation_name);
      }
    }

    /* IDs include WordNet version and part-of-speech */
    synsetElem->add_child("ID")->set_child_text("eng-30-" + synsetId + "-" + pos);

    /* Every literal, or _EMPTY_ if the synset isn't translated */
    xmlpp::Element* synonymElem = synsetElem->add_child("SYNONYM");
    if (wne.frenchSynset.empty()) {
      synonymElem->add_child("LITERAL")->set_child_text("_EMPTY_");
    } else {
      for (const auto& itfs : wne.frenchSynset) {
        std::string escaped_translation = escape_xml(itfs.first);
        xmlpp::Element* literalElem = synonymElem->add_child("LITERAL");
        literalElem->set_child_text(escaped_translation);

        std::map<std::string, float> scores;
        /* Group scores by processor module */
        for (const auto& ittranslationinfo: itfs.second) {
          scores[ittranslationinfo.processed] += ittranslationinfo.score;
        }

        std::string lnote;
        for (const auto& itscore: scores) {
          lnote += "wonef-" + itscore.first + "(" + boost::lexical_cast<std::string>(itscore.second) + ")" + ";";
        }
        lnote.erase(lnote.size()-1); // removes the extra ";" at the end
        literalElem->set_attribute("lnote", lnote);
      }
    }
   /* Temporary solution to display candidates */
   /*
   for (const auto& itfc: wne.frenchCandidates) {
     for (const auto& itcand: itfc.second.cand) {
       synonymElem->add_child("CANDIDATE")->set_child_text(itcand.first);
     }
   }
   */


    /* definition and usage */
    synsetElem->add_child("DEF")->set_child_text(boost::trim_copy(wne.def));
    for(auto usage: wne.usages) {
      synsetElem->add_child("USAGE")->set_child_text(usage);
    }

    /* Is it in a Basic Concept Set? */
    if (bcsbase.find(synsetId) != bcsbase.end()) {
      synsetElem->add_child("BCS")->set_child_text(boost::lexical_cast<std::string>(bcsbase[synsetId]));
    }

    /* part-of-speech */
    synsetElem->add_child("POS")->set_child_text(pos);
  }

  document.write_to_file_formatted(datafile, "UTF-8");

}



