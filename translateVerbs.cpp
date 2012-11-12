#include "../src/knn_search.h"
//#include "../src/converter.h"
#include "Options.hpp"
#include "Paths.hpp"
#include "Loader.hpp"
#include "Dumper.hpp"
#include "Timer.hpp"

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

  Timer globalTimer;

  SimSynModule* simsyner = NULL;
  HyperHypoModule* hyperhypoer= NULL;
  LastChanceModule* lastchancer= NULL;

  string pos = "verb";
  Options options(pos, argc, argv);

  cout << "Init " << options.suffix << endl;

  Timer t;
  cout << "Loading... ";
  LoaderModule loader(options.datafile, VERBS_P_LIST, pos);
  WORDNET::WordNet wn = loader.load(false, -1); // verbose false
  cout << t.duration() << "s" << endl;

  t.start();
  ExtractorModule extractor(pos, options);
  extractor.process(wn);
  cout << "Extraction duration : " << t.duration() << "s" << endl;

  int nIteration = 0;
  for(int idModuleConf: options.moduleSequence) {
    nIteration++;
    switch (idModuleConf) {
    case 1:
      cout << "First step " << endl;
      t.start();
      lastchancer = new LastChanceModule(idModuleConf, nIteration);
      lastchancer->process(wn);
      delete lastchancer;
      cout << "First step duration : " << t.duration() << "s" << endl;
      break;

    case 2 :
      cout << "Second step "  << endl;
      t.start();
      simsyner = new SimSynModule(pos, idModuleConf, nIteration);
      simsyner->process(wn);
      delete simsyner;
      cout << "Second step duration : " << t.duration() << "s" << endl;
      break;

    case 3 :
      cout << "Third step "  << endl;
      t.start();
      hyperhypoer = new HyperHypoModule(options.datafile, "SUJ_V_RELG.reverse", R_HYPER, pos, idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Third step duration : " << t.duration() << "s" << endl;
      break;

    case 4 :
      cout << "Fourth step "  << endl;
      t.start();
      hyperhypoer = new HyperHypoModule(options.datafile, "COD_V.reverse", R_HYPER, pos, idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Fourth step duration : " << t.duration() << "s" << endl;
      break;

    case 5 :
      cout << "Fifth step "  << endl;
      t.start();
      hyperhypoer = new HyperHypoModule(options.datafile, "CPL_V.reverse", R_HYPER, pos, idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Fifth step duration : " << t.duration() << "s" << endl;
      break;

    case 6 :
      cout << "Six step "  << endl;
      t.start();
      hyperhypoer = new HyperHypoModule(options.datafile, "ATB_S.reverse", R_HYPER, pos, idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Six step duration : " << t.duration() << "s" << endl;
      break;

    case 7 :
      cout << "Seventh step "  << endl;
      t.start();
      hyperhypoer = new HyperHypoModule(options.datafile, "SUJ_V.reverse", R_HYPER, pos, idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Seventh step duration : " << t.duration() << "s" << endl;
      break;

    case 8 :
      cout << "Eigth step "  << endl;
      t.start();
      hyperhypoer = new HyperHypoModule(options.datafile, "CPLV_V.reverse", R_HYPER, pos, idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Eigth step duration : " << t.duration() << "s" << endl;
      break;

    default:
      break;
    }
  }

  cout << "Print Index  " << endl;
  t.start();
  DumperModule dumper("data/jaws.verb" + options.suffix + ".xml");
  dumper.dump(wn);
  cout << "Print index duration : " << t.duration() << "s" << endl;

  cout << "Choose best translations" << endl;
  t.start();
  BestTranslations(options).choose(wn);
  cout << "Choice duration : " << t.duration() << "s" << endl;

  cout << "Print best JAWS" << endl;
  t.start();
  DumperModule dumperBest("data/jaws.best.verb" + options.suffix + ".xml");
  dumperBest.dump(wn);
  cout << "Printing best JAWS duration : " << t.duration() << "s" << endl;


  cout << "Overall duration : " << globalTimer.duration() << "s" << endl;
  return 0;
}
