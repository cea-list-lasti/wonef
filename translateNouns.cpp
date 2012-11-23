#include "../src/knn_search.h"
#include "Paths.hpp"
#include "Options.hpp"
#include "Loader.hpp"
#include "Dumper.hpp"
#include "DEBVisDicDumper.hpp"
#include "Timer.hpp"

// module heuristics
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

  string pos = "noun";
  Options options(pos, argc, argv);

  cout << "Init " << options.suffix << endl;

  Timer t;
  LoaderModule loader(options.datafile, NOUNS_LIST, pos);
  WORDNET::WordNet wn = loader.load(false, -1); // verbose false
  cout << "Loading duration : " << t.duration() << "s" << endl;

  t.start();
  ExtractorModule extractor(pos, options);
  extractor.process(wn);
  cout << "Extraction duration : " << t.duration() << "s" << endl;

  //BestTranslations(options, true).choose(wn);

  int nIteration = 0;
  for(int idModuleConf: options.moduleSequence) {
    nIteration++;
    switch (idModuleConf) {
    case 1:
      cout << "First step "  << endl;
      t.start();
      SimSynModule(pos, idModuleConf, nIteration).process(wn);
      cout << "First step duration : " << t.duration() << "s" << endl;
      break;

    case 2 :
      cout << "Second step "  << endl;
      t.start();
      HyperHypoModule(options.datafile, "COMPDUNOM", R_HYPER, pos, idModuleConf, nIteration).process(wn);
      cout << "Second step duration : " << t.duration() << "s" << endl;
      break;

    case 3 :
      cout << "Third step " << endl;
      t.start();
      MeroHoloLikeHyperModule(options.datafile, idModuleConf, nIteration).process(wn, R_HYPER);
      cout << "Third step duration : " << t.duration() << "s" << endl;
      break;

    case 4 :
      cout << "Fourth step " << endl;
      t.start();
      MeroHoloModule(options.datafile, idModuleConf, nIteration).process(wn);
      cout << "Fourth step duration : " << t.duration() << "s" << endl;
      break;

    case 5 :
      cout << "Fifth step does not exist" << endl;
      break;

    case 6 :
      cout << "Six step "  << endl;
      t.start();
      HyperHypoModule(options.datafile, "SUJ_V", R_HYPER, pos, idModuleConf, nIteration).process(wn);
      cout << "Six step duration : " << t.duration() << "s" << endl;
      break;

    case 7 :
      cout << "Seventh step "  << endl;
      t.start();
      HyperHypoModule(options.datafile, "COMPDUNOM", R_HYPO, pos, idModuleConf, nIteration).process(wn);
      cout << "Seventh step duration : " << t.duration() << "s" << endl;
      break;

    case 8 :
      cout << "Eigth step "  << endl;
      t.start();
      HyperHypoModule(options.datafile, "COD_V", R_HYPER, pos, idModuleConf, nIteration).process(wn);
      cout << "Eigth step duration : " << t.duration() << "s" << endl;
      break;

    case 9 :
      cout << "Ninth step "  << endl;
      t.start();
      HyperHypoModule(options.datafile, "window10", R_HYPER, pos, idModuleConf, nIteration).process(wn);
      cout << "Ninth step duration : " << t.duration() << "s" << endl;
      break;

    default:
      break;
    }

  }

  cout << "Print Index  " << endl;
  t.start();
  DumperModule dumper("data/jaws.noun" + options.suffix + ".xml");
  dumper.dump(wn);
  cout << "Print index duration : " << t.duration() << "s" << endl;

  cout << "Choose best translations" << endl;
  t.start();
  BestTranslations(options).choose(wn);
  cout << "Choice duration : " << t.duration() << "s" << endl;

  cout << "Print best JAWS" << endl;
  t.start();
  DumperModule dumperBest("data/jaws.best.noun" + options.suffix + ".xml");
  dumperBest.dump(wn);
  DEBVisDicDumperModule("data/jaws.best.noun" + options.suffix + ".deb.xml").dump(wn);
  cout << "Printing best JAWS duration : " << t.duration() << "s" << endl;


  cout << "Overall duration : " << globalTimer.duration() << "s" << endl;
  return 0;
}
