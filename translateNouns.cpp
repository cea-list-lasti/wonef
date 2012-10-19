#include "../src/knn_search.h"
//#include "../src/converter.h"
#include "Paths.hpp"
#include "Options.hpp"
#include "Loader.hpp"
#include "Dumper.hpp"
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

  SimSynModule* simsyner = NULL;
  HyperHypoModule* hyperhypoer= NULL;
  MeroHoloModule* meroholoer= NULL;
  MeroHoloLikeHyperModule* merohololiker= NULL;
  LastChanceModule* lastchancer= NULL;

  set<string> dicfiles{DICFILE, DICFILE2};
  string pos = "noun";
  Options options(pos, argc, argv);

  cout << "Init " << options.suffix << endl;

  Timer t;
  LoaderModule loader(options.datafile, dicfiles, NOUNS_LIST, pos);
  WORDNET::WordNet wn = loader.load(false, -1); // verbose false
  cout << "Loading duration : " << t.duration() << "s" << endl;

  t.start();
  ExtractorModule extractor(pos, options.extractionSet);
  extractor.process(wn);
  cout << "Extraction duration : " << t.duration() << "s" << endl;

  int nIteration = 0;
  for(int idModuleConf: options.moduleSequence) {
    nIteration++;
    switch (idModuleConf) {
    case 1:
      cout << "First step "  << endl;
      t.start();
      simsyner = new SimSynModule(pos, idModuleConf, nIteration);
      simsyner->process(wn);
      delete simsyner;
      cout << "First step duration : " << t.duration() << "s" << endl;
      break;

    case 2 :
      cout << "Second step "  << endl;
      t.start();
      hyperhypoer = new HyperHypoModule(options.datafile, "COMPDUNOM", R_HYPER, pos, idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Second step duration : " << t.duration() << "s" << endl;
      break;

    case 3 :
      cout << "Third step " << endl;
      t.start();
      merohololiker = new MeroHoloLikeHyperModule(options.datafile, idModuleConf, nIteration);
      merohololiker->process(wn, R_HYPER);
      delete merohololiker;
      cout << "Third step duration : " << t.duration() << "s" << endl;
      break;

    case 4 :
      cout << "Fourth step " << endl;
      t.start();
      meroholoer = new MeroHoloModule(options.datafile, idModuleConf, nIteration);
      meroholoer->process(wn);
      //      meroholoer->finalize();
      delete meroholoer;
      cout << "Fourth step duration : " << t.duration() << "s" << endl;
      break;

    case 5 :
      cout << "Fifth step " << endl;
      t.start();
      lastchancer = new LastChanceModule(idModuleConf, nIteration);
      lastchancer->process(wn);
      delete lastchancer;
      cout << "Fifth step duration : " << t.duration() <<  "s" << endl;
      break;

    case 6 :
      cout << "Six step "  << endl;
      t.start();
      hyperhypoer = new HyperHypoModule(options.datafile, "SUJ_V", R_HYPER, pos, idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Six step duration : " << t.duration() << "s" << endl;
      break;

    case 7 :
      cout << "Seventh step "  << endl;
      hyperhypoer = new HyperHypoModule(options.datafile, "COMPDUNOM", R_HYPO, pos, idModuleConf, nIteration);
      t.start();
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Seventh step duration : " << t.duration() << "s" << endl;
      break;

    case 8 :
      cout << "Eigth step "  << endl;
      t.start();
      hyperhypoer = new HyperHypoModule(options.datafile, "COD_V", R_HYPER, pos, idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Eigth step duration : " << t.duration() << "s" << endl;
      break;

    case 9 :
      cout << "Ninth step "  << endl;
      t.start();
      hyperhypoer = new HyperHypoModule(options.datafile, "window10", R_HYPER, pos, idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Ninth step duration : " << t.duration() << "s" << endl;
      break;

    default:
      break;
    }

  }

  cout << "Print Index  " << endl;
  t.start();
  DumperModule dumper("data2/data.fr.nouns" + options.suffix, "data2/index.fr.noun" + options.suffix);
  dumper.dump(wn);
  cout << "Print index duration : " << t.duration() << "s" << endl;

  cout << "Choose best translations" << endl;
  t.start();
  BestTranslations bestTranslations;
  bestTranslations.choose(wn);
  cout << "Choice duration : " << t.duration() << "s" << endl;

  cout << "Print best JAWS" << endl;
  t.start();
  DumperModule dumperBest("data2/data.fr.nouns.best" + options.suffix, "data2/index.fr.nouns.best" + options.suffix);
  dumperBest.dump(wn);
  cout << "Printing best JAWS duration : " << t.duration() << "s" << endl;


  cout << "Overall duration : " << globalTimer.duration() << "s" << endl;
  return 0;
}
