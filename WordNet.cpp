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

  POS pos;

}
