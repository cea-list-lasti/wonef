#include "WordNet.hpp"

namespace WORDNET {

  std::map<std::string, POS> POS_of_string = {
    {"noun", POS::Noun},
    {"verb", POS::Verb},
    {"adj", POS::Adj},
    {"adv", POS::Adv}};

  std::map<POS, std::string> string_of_POS = {
    {POS::Noun, "noun"},
    {POS::Verb, "verb"},
    {POS::Adj, "adj"},
    {POS::Adv, "adv"}};

  std::map<POS, std::string> BalkaNet_string_of_POS = {
    {POS::Noun, "n"},
    {POS::Verb, "v"},
    {POS::Adj, "a"},
    {POS::Adv, "b"}};

  std::map<std::string, std::string> ILR_of_code = {
    {"@", "hypernym"},
    {"+", "eng_derivative"},
    {"&", "similar_to"},
    {"#m", "holo_member"},
    {"#p", "holo_part"},
    {"@i", "instance_hypernym"},
    {"!", "near_antonym"},
    {"\\", "derived"},
    {";c", "category_domain"},
    {"^", "also_sees"},
    {"$", "verb_group"},
    {";r", "region_domain"},
    {";u", "usage_domain"},
    {"#s", "holo_portion"},
    {">", "causes"},
    {"<", "participle"},
    {"=", "be_in_state"},
    {"*", "subevent"},
    // in NLTK but not in WOLF
    {"~", "hyponym"},
    {"~i", "instance_hyponym"},
    {"#s", "substance_holonym"},
    {"%m", "member_meronym"},
    {"%s", "substance_meronym"},
    {"%p", "part_meronym"}};

  std::set<std::string> child_relations = {
    "hyponym",
    "instance_hyponym",
    "substance_holonym",
    "member_meronym",
    "substance_meronym",
    "part_meronym"};

  POS pos;
}
