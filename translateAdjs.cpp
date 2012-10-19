#include "../src/knn_search.h"
//#include "../src/converter.h"
#include "Paths.hpp"
#include "Options.hpp"
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

  Timer globalStart;

  SimSynModule* simsyner = NULL;
  LastChanceModule* lastchancer= NULL;

  string pos = "adj";
  set<string> dicfiles{DICFILE, DICFILE2};
  Options options(pos, argc, argv);

  cout << "Init " << options.suffix << endl;

  Timer t;
  LoaderModule loader(options.datafile, dicfiles, ADJS_LIST, pos);
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

    default:
      break;
    }

  }

  cout << "Print Index  " << endl;
  t.start();
  DumperModule dumper("data2/data.fr.adjs" + options.suffix, "data2/index.fr.adjs" + options.suffix);
  dumper.dump(wn);
  cout << "Print index duration : " << t.duration() << "s" << endl;

  cout << "Choose best translations" << endl;
  t.start();
  BestTranslations bestTranslations;
  bestTranslations.choose(wn);
  cout << "Choice duration : " << t.duration() << "s" << endl;

  cout << "Print best JAWS" << endl;
  t.start();
  DumperModule dumperBest("data2/data.fr.adjs.best" + options.suffix, "data2/index.fr.adjs.best" + options.suffix);
  dumperBest.dump(wn);
  cout << "Printing best JAWS duration : " << t.duration() << "s" << endl;


  cout << "Overall duration : " << globalStart.duration() << "s" << endl;
  return 0;
}
