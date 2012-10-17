#ifndef PATHS_HPP
#define PATHS_HPP

/*
const std::string  INDEX_VERB "/data/text/WordNet-3.0/dict/index.verb";
const std::string  DATA_VERB "/data/text/WordNet-3.0/dict/data.verb";
const std::string  INDEX_ADJ "/data/text/WordNet-3.0/dict/index.adj";
const std::string  DATA_ADJ "/data/text/WordNet-3.0/dict/data.adj";
*/

const std::string  DATAPATH = "/home/pradet/data";

const std::string  DICFILE = DATAPATH + "/dictionnaires/euradic-utf8.txt";
const std::string  DICFILE2 = DATAPATH + "/dictionnaires/frwiktionary-20090120-eldafmt.csv";

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

const std::string  KNNSTDDIR = DATAPATH + "/FastKNN/knnall/";
const std::string  KNNSTDFILE = "KNNSTDDIR/$REL/SemMap.$REL.16384.knnResFile.$WORD.txt";


const std::string  TYPEROLERFILE13 = DATAPATH + "/FastKNN/SemMap/reverse/SUJ_V_RELG.reverse.dmp";
const std::string  TYPEROLERFILE12 = DATAPATH + "/FastKNN/SemMap/reverse/MOD_V.reverse.dmp";
const std::string  TYPEROLERFILE11 = DATAPATH + "/FastKNN/SemMap/normal/CPLV_V.dmp";
const std::string  TYPEROLERFILE10 = DATAPATH + "/FastKNN/SemMap/reverse/CPLV_V.reverse.dmp";
const std::string  TYPEROLERFILE9  = DATAPATH + "/FastKNN/SemMap/reverse/CPL_V.reverse.dmp";
const std::string  TYPEROLERFILE8  = DATAPATH + "/FastKNN/SemMap/reverse/AdvVerbe.reverse.dmp";
const std::string  TYPEROLERFILE7  = DATAPATH + "/FastKNN/SemMap/reverse/ATB_S.reverse.dmp";
const std::string  TYPEROLERFILE6  = DATAPATH + "/FastKNN/SemMap/reverse/COD_V.reverse.dmp";
const std::string  TYPEROLERFILE5  = DATAPATH + "/FastKNN/SemMap/reverse/SUJ_V.reverse.dmp";
const std::string  TYPEROLERFILE4  = DATAPATH + "/FastKNN/SemMap/windows/window10.dmp";
const std::string  TYPEROLERFILE3  = DATAPATH + "/FastKNN/SemMap/normal/COD_V.dmp";
const std::string  TYPEROLERFILE2  = DATAPATH + "/FastKNN/SemMap/normal/SUJ_V.dmp";
const std::string  TYPEROLERFILE   = DATAPATH + "/FastKNN/SemMap/normal/COMPDUNOM.dmp";
const std::string  HOLOFILE        = DATAPATH + "/FastKNN/SemMap/reverse/COMPDUNOM.reverse.dmp";

const std::string  WORDS_IDS = DATAPATH + "/FastKNN/SemMap/words.ids";
const std::string  CONVFILE = DATAPATH + "/FastKNN/SemMap/semmapconversion.global.bin";

const std::string  MAPVERB20_15 = DATAPATH + "/opendata/mappings-upc-2007/mapping-20-15/wn20-15.verb";
const std::string  MAPVERB15_20 = DATAPATH + "/opendata/mappings-upc-2007/mapping-15-20/wn15-20.verb";

const std::string  KNNPROTODIR = "/tmp/protobuf/";
const std::string  KNNPROTOFILE = KNNPROTODIR + "knndists_";

const std::string  BCSFILE = DATAPATH + "/opendata/wolf/wolf-0.1.5.format.xml";
const std::string MAPPING_30_20 = DATAPATH + "/opendata/mappings-upc-2007/mapping-30-20/wn30-20.";
const std::string MAPPING_20_30 = DATAPATH + "/opendata/mappings-upc-2007/mapping-20-30/wn20-30.";

#endif
