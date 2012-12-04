#ifndef PATHS_HPP
#define PATHS_HPP

//#define CONVFILE "/data/text/FastKNN/bases/semmapconversion.global.bin"
//#define DICFILE "/data/text/FrameNetLUI/externals/dicfrenelda-utf8.txt"
/*#define INDEX_NOUN "/data/text/WordNet-3.0/dict/index.nounk"
#define DATA_NOUN "/data/text/WordNet-3.0/dict/data.noun"
#define NOUNS_LIST "/data/text/FrameNetLUI/externals/noms.list"

#define INDEX_VERB "/data/text/WordNet-3.0/dict/index.verb"
#define DATA_VERB "/data/text/WordNet-3.0/dict/data.verb"
#define INDEX_ADJ "/data/text/WordNet-3.0/dict/index.adj"
#define DATA_ADJ "/data/text/WordNet-3.0/dict/data.adj"
*/

#define EURADIC "/data/text/FrameNetLUI/externals/dicfrenelda-utf8.txt"
#define WIKTIONARY "/data/text/quentin/opendata/frwiktionary-20090120-eldafmt.stripped.csv"

const std::string DATAPATH = "/data/text/quentin";

const std::string GOLD_NOUN = DATAPATH + "/Gold30/GT_nouns30.xml";
const std::string GOLD_VERB = DATAPATH + "/Gold30/GT_verbs30.xml";
const std::string GOLD_ADJ = DATAPATH + "/Gold30/GT_adjs30.xml";


const std::string WOLF = DATAPATH + "/opendata/wolf/wolf-0.1.5.format.xml";
const std::string WOLFONE = DATAPATH + "/opendata/wolf10b/wolf-1.0b.xml";
const std::string EWN = DATAPATH + "/opendata/ewn/wn_fr.ewn.utf8";
const std::string POLYSEMOUSINDEX = DATAPATH + "/opendata/polysemous/WordNet-3.0/index.polysemous.";
const std::string BCSFILE = DATAPATH + "/opendata/wolf/wolf-0.1.5.format.xml";
const std::string MAPPING_30_20 = DATAPATH + "/opendata/mappings-upc-2007/mapping-30-20/wn30-20.";
const std::string MAPPING_20_30 = DATAPATH + "/opendata/mappings-upc-2007/mapping-20-30/wn20-30.";

const std::string DATA_NOUN30 = DATAPATH + "/opendata/WordNet-3.0/dict/data.noun";
const std::string DATA_NOUN20 = DATAPATH + "/opendata/WordNet-2.0/dict/data.noun";
const std::string DATA_NOUN15 = DATAPATH + "/opendata/WordNet-1.5/DICT/NOUN.DAT";
const std::string NOUNS_LIST = DATAPATH + "/POSlist/noms.list";

const std::string DATA_VERB30 = DATAPATH + "/opendata/WordNet-3.0/dict/data.verb";
const std::string DATA_VERB20 = DATAPATH + "/opendata/WordNet-2.0/dict/data.verb";
const std::string DATA_VERB15 = DATAPATH + "/opendata/WordNet-1.5/DICT/VERB.DAT";
const std::string VERBS_LIST = DATAPATH + "/POSlist/verbes.list";
const std::string VERBS_P_LIST = DATAPATH + "/POSlist/verbes.p.list";

const std::string DATA_ADJ30 = DATAPATH + "/opendata/WordNet-3.0/dict/data.adj";
const std::string DATA_ADJ20 = DATAPATH + "/opendata/WordNet-2.0/dict/data.adj";
const std::string DATA_ADJ15 = DATAPATH + "/opendata/WordNet-1.5/DICT/ADJ.DAT";
const std::string ADJS_LIST = DATAPATH + "/POSlist/adjectifs.list";
const std::string ADJS_LIST_F = DATAPATH + "/POSlist/adjectifs.filtered.list";

const std::string DATA_ADV30 = DATAPATH + "/opendata/WordNet-3.0/dict/data.adv";
const std::string DATA_ADV20 = DATAPATH + "/opendata/WordNet-2.0/dict/data.adv";
const std::string DATA_ADV15 = DATAPATH + "/opendata/WordNet-1.5/DICT/ADV.DAT";
const std::string ADVS_LIST = DATAPATH + "/POSlist/others.uniq.list";
const std::string ADVS_LIST_F = DATAPATH + "/POSlist/others.uniq.filtered.list";

#define KNNSTDFILE "/data/text/FastKNN/knnall/$REL/SemMap.$REL.16384.knnResFile.$WORD.txt"
#define KNNSTDDIR "/data/text/FastKNN/knnall/"

#define WORDS_IDS "/data/text/FastKNN/Ressources/SemMap/words.ids"

#define TYPEROLERFILE13 "/data/text/FastKNN/Ressources/SemMap/reverse/SUJ_V_RELG.reverse.dmp"
#define TYPEROLERFILE12 "/data/text/FastKNN/Ressources/SemMap/reverse/MOD_V.reverse.dmp"
#define TYPEROLERFILE11 "/data/text/FastKNN/Ressources/SemMap/normal/CPLV_V.dmp"
#define TYPEROLERFILE10 "/data/text/FastKNN/Ressources/SemMap/reverse/CPLV_V.reverse.dmp"
#define TYPEROLERFILE9 "/data/text/FastKNN/Ressources/SemMap/reverse/CPL_V.reverse.dmp"
#define TYPEROLERFILE8 "/data/text/FastKNN/Ressources/SemMap/reverse/AdvVerbe.reverse.dmp"
#define TYPEROLERFILE7 "/data/text/FastKNN/Ressources/SemMap/reverse/ATB_S.reverse.dmp"
#define TYPEROLERFILE6 "/data/text/FastKNN/Ressources/SemMap/reverse/COD_V.reverse.dmp"
#define TYPEROLERFILE5 "/data/text/FastKNN/Ressources/SemMap/reverse/SUJ_V.reverse.dmp"
#define TYPEROLERFILE4 "/data/text/FastKNN/Ressources/SemMap/windows/window10.dmp"
#define TYPEROLERFILE3 "/data/text/FastKNN/Ressources/SemMap/normal/COD_V.dmp"
#define TYPEROLERFILE2 "/data/text/FastKNN/Ressources/SemMap/normal/SUJ_V.dmp"
#define TYPEROLERFILE "/data/text/FastKNN/Ressources/SemMap/normal/COMPDUNOM.dmp"
#define HOLOFILE "/data/text/FastKNN/Ressources/SemMap/reverse/COMPDUNOM.reverse.dmp"

#define CONVFILE "/data/text/FastKNN/bases.fr/semmapconversion.global.bin"

#define MAPVERB20_15 "/data/text/jeanne/mappings-upc-2007/mapping-20-15/wn20-15.verb"
#define MAPVERB15_20 "/data/text/jeanne/mappings-upc-2007/mapping-15-20/wn15-20.verb"

#define KNNPROTODIR "/tmp/protobuf/"
#define KNNPROTOFILE "/tmp/protobuf/knndists_"

#endif
