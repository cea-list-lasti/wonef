#ifndef PATHS_HPP
#define PATHS_HPP

#include <string>

/*
const std::string  INDEX_VERB "/data/text/WordNet-3.0/dict/index.verb";
const std::string  DATA_VERB "/data/text/WordNet-3.0/dict/data.verb";
const std::string  INDEX_ADJ "/data/text/WordNet-3.0/dict/index.adj";
const std::string  DATA_ADJ "/data/text/WordNet-3.0/dict/data.adj";
*/

const std::string DATAPATH = DATAPATH_CMAKE;

const std::string WOLF = DATAPATH + "/opendata/wolf/wolf-0.1.5.format.xml";
const std::string WOLFONE = DATAPATH + "/opendata/wolf10b/wolf-1.0b.xml";
const std::string EWN = DATAPATH + "/opendata/ewn/wn_fr.ewn.utf8";

const std::string GOLD_NOUN = DATAPATH + "/Gold30/GT_nouns30.test.xml";
const std::string GOLD_VERB = DATAPATH + "/Gold30/GT_verbs30.test.xml";
const std::string GOLD_ADJ = DATAPATH + "/Gold30/GT_adjs30.test.xml";
const std::string GOLD_ADV = DATAPATH + "/Gold30/GT_adv30.test.xml";

const std::string GOLD_DEV_NOUN = DATAPATH + "/Gold30/GT_nouns30.dev.xml";
const std::string GOLD_DEV_VERB = DATAPATH + "/Gold30/GT_verbs30.dev.xml";
const std::string GOLD_DEV_ADJ = DATAPATH + "/Gold30/GT_adjs30.dev.xml";
const std::string GOLD_DEV_ADV = DATAPATH + "/Gold30/GT_adv30.dev.xml";

const std::string EURADIC = DATAPATH + "/dictionnaires/euradic-utf8.txt";
const std::string WIKTIONARY = DATAPATH + "/dictionnaires/frwiktionary-20090120-eldafmt.csv";

const std::string  DATA_NOUN30 = DATAPATH + "/opendata/WordNet-3.0/dict/data.noun";
const std::string  DATA_NOUN20 = DATAPATH + "/opendata/WordNet-2.0/dict/data.noun";
const std::string  DATA_NOUN15 = DATAPATH + "/opendata/WordNet-1.5/DICT/NOUN.DAT";
const std::string  NOUNS_LIST = DATAPATH + "/POSlist/noms.list";

const std::string  DATA_VERB30 = DATAPATH + "/opendata/WordNet-3.0/dict/data.verb";
const std::string  DATA_VERB20 = DATAPATH + "/opendata/WordNet-2.0/dict/data.verb";
const std::string  DATA_VERB15 = DATAPATH + "/opendata/WordNet-1.5/DICT/VERB.DAT";
const std::string  VERBS_LIST = DATAPATH + "/POSlist/verbes.list";
const std::string  VERBS_P_LIST = DATAPATH + "/POSlist/verbes.p.list";

const std::string  DATA_ADJ30 = DATAPATH + "/opendata/WordNet-3.0/dict/data.adj";
const std::string  DATA_ADJ20 = DATAPATH + "/opendata/WordNet-2.0/dict/data.adj";
const std::string  DATA_ADJ15 = DATAPATH + "/opendata/WordNet-1.5/DICT/ADJ.DAT";
const std::string  ADJS_LIST = DATAPATH + "/POSlist/adjectifs.list";
const std::string  ADJS_LIST_F = DATAPATH + "/POSlist/adjectifs.filtered.list";

const std::string DATA_ADV30 = DATAPATH + "/opendata/WordNet-3.0/dict/data.adv";
const std::string DATA_ADV20 = DATAPATH + "/opendata/WordNet-2.0/dict/data.adv";
const std::string DATA_ADV15 = DATAPATH + "/opendata/WordNet-1.5/DICT/ADV.DAT";
const std::string ADVS_LIST = DATAPATH + "/POSlist/others.uniq.list";
const std::string ADVS_LIST_F = DATAPATH + "/POSlist/others.uniq.filtered.list";

const std::string  KNNSTDDIR = DATAPATH + "/FastKNN/knnall/";
const std::string  KNNSTDFILE = "KNNSTDDIR/$REL/SemMap.$REL.16384.knnResFile.$WORD.txt";

const std::string  WORDS_IDS = DATAPATH + "/FastKNN/SemMap/words.ids";
const std::string  CONVFILE = DATAPATH + "/FastKNN/SemMap/semmapconversion.global.bin";

const std::string  MAPVERB20_15 = DATAPATH + "/opendata/mappings-upc-2007/mapping-20-15/wn20-15.verb";
const std::string  MAPVERB15_20 = DATAPATH + "/opendata/mappings-upc-2007/mapping-15-20/wn15-20.verb";

const std::string  KNNPROTODIR = DATAPATH + "/protobuf/";
const std::string  KNNPROTOFILE = KNNPROTODIR + "knndists_";

const std::string  BCSFILE = DATAPATH + "/opendata/wolf/wolf-0.1.5.format.xml";
const std::string MAPPING_30_20 = DATAPATH + "/opendata/mappings-upc-2007/mapping-30-20/wn30-20.";
const std::string MAPPING_20_30 = DATAPATH + "/opendata/mappings-upc-2007/mapping-20-30/wn20-30.";

const std::string POLYSEMOUSINDEX = DATAPATH + "/opendata/polysemous/WordNet-3.0/index.polysemous.";

#endif
