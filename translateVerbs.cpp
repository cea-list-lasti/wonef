#include "../src/knn_search.h"
//#include "../src/converter.h"
#include "Options.hpp"
#include "Paths.hpp"
#include "Loader.hpp"
#include "Dumper.hpp"
#include "ExtractorModule.hpp"
#include "SimSynModule.hpp"
#include "HyperHypoModule.hpp"
#include "MeroHoloModule.hpp"
#include "MeroHoloLikeHyperModule.hpp"
#include "LastChanceModule.hpp"
#include "BestTranslations.hpp"
#include <set>

using namespace std;


int main(int argc, char **argv) {

  time_t globalStart = time(NULL);

  SimSynModule* simsyner = NULL;
  HyperHypoModule* hyperhypoer= NULL;
  LastChanceModule* lastchancer= NULL;

  string pos = "verb";

  Options options(pos, argc, argv);

  cout << "Init " << options.suffix << endl;
  time_t start = time(NULL);

  set<string> dicfiles{DICFILE, DICFILE2};

  LoaderModule loader(options.datafile, dicfiles, VERBS_P_LIST, pos, options.noen);
  WORDNET::WordNet wn = loader.load(false, -1); // verbose false
  cout << "Init duration : " << time(NULL) - start << " s " << endl;

  start = time(NULL);
  std::set<ExtractionType> extractions{ExtractionType::Monosemous, ExtractionType::NoTranslation, ExtractionType::Uniq};
  ExtractorModule extractor(pos, extractions);
  extractor.process(wn);
  cout << "Extraction duration : " << time(NULL) - start << " s " << endl;

  int nIteration = 0;
  for(int idModuleConf: options.sequence) {
    nIteration++;
    switch (idModuleConf) {
    case 1:
      cout << "First step " << endl;
      start = time(NULL);
      lastchancer = new LastChanceModule(idModuleConf, nIteration);
      lastchancer->process(wn);
      delete lastchancer;
      cout << "First step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 2 :
      cout << "Second step "  << endl;
      start = time(NULL);
      simsyner = new SimSynModule(pos, idModuleConf, nIteration);
      simsyner->process(wn);
      delete simsyner;
      cout << "Second step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 3 :
      cout << "Third step "  << endl;
      start = time(NULL);
      hyperhypoer = new HyperHypoModule(options.datafile, "SUJ_V_RELG.reverse", R_HYPER, pos, idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Third step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 4 :
      cout << "Fourth step "  << endl;
      start = time(NULL);
      hyperhypoer = new HyperHypoModule(options.datafile, "COD_V.reverse", R_HYPER, pos, idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Fourth step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 5 :
      cout << "Fifth step "  << endl;
      start = time(NULL);
      hyperhypoer = new HyperHypoModule(options.datafile, "CPL_V.reverse", R_HYPER, pos, idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Fifth step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 6 :
      cout << "Six step "  << endl;
      start = time(NULL);
      hyperhypoer = new HyperHypoModule(options.datafile, "ATB_S.reverse", R_HYPER, pos, idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Six step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 7 :
      cout << "Seventh step "  << endl;
      start = time(NULL);
      hyperhypoer = new HyperHypoModule(options.datafile, "SUJ_V.reverse", R_HYPER, pos, idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Seventh step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 8 :
      cout << "Eigth step "  << endl;
      start = time(NULL);
      hyperhypoer = new HyperHypoModule(options.datafile, "CPLV_V.reverse", R_HYPER, pos, idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Eigth step duration : " << time(NULL) - start << " s " << endl;
      break;

    default:
      break;
    }
  }

  cout << "Print Index  " << endl;
  start = time(NULL);
  DumperModule dumper("data2/data.fr.verbs" + options.suffix, "data2/index.fr.verbs" + options.suffix);
  dumper.dump(wn);
  cout << "Print index duration : " << time(NULL) - start << " s " << endl;

  cout << "Choose best translations" << endl;
  start = time(NULL);
  BestTranslations bestTranslations;
  bestTranslations.choose(wn);
  cout << "Choice duration : " << time(NULL) - start << " s " << endl;

  cout << "Print best JAWS" << endl;
  start = time(NULL);
  DumperModule dumperBest("data2/data.fr.verbs.best" + options.suffix, "data2/index.fr.verbs.best" + options.suffix);
  dumperBest.dump(wn);
  cout << "Printing best JAWS duration : " << time(NULL) - start << " s " << endl;


  cout << "Overall duration : " << time(NULL) - globalStart << " s " << endl;
  return 0;
}
