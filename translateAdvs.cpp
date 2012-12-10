#include "../src/knn_search.h"
//#include "../src/converter.h"
#include "Paths.hpp"
#include "Options.hpp"
#include "Loader.hpp"
#include "Dumper.hpp"
#include "DEBVisDicDumper.hpp"
#include "Timer.hpp"

#include "SimSynModule.hpp"
#include "BestTranslations.hpp"

#include <set>

using namespace std;


int main(int argc, char **argv) {

  Timer globalStart;

  string pos = "adv";
  WORDNET::pos = WORDNET::POS_of_string[pos];
  Options options(pos, argc, argv);

  cout << "Init " << options.suffix << endl;

  Timer t;
  cout << "Loading... ";
  LoaderModule loader(options.datafile, ADVS_LIST, pos);
  WORDNET::WordNet wn = loader.load();
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
      cout << "First step "  << endl;
      t.start();
      SimSynModule(pos, idModuleConf, nIteration).process(wn);
      cout << "First step duration : " << t.duration() << "s" << endl;
      break;

    default:
      break;
    }

  }

  cout << "Dumping Jaws...";
  t.start();
  DumperModule dumper("data/jaws.adv" + options.suffix + ".xml");
  dumper.dump(wn);
  cout << t.duration() << "s" << endl;

  cout << "Choosing best translations...";
  t.start();
  BestTranslations(options).choose(wn);
  cout << t.duration() << "s" << endl;

  cout << "Print best JAWS" << endl;
  t.start();
  DumperModule dumperBest("data/jaws.best.adv" + options.suffix + ".xml");
  dumperBest.dump(wn);
  DEBVisDicDumperModule("data/jaws.best.adv" + options.suffix + ".deb.xml").dump(wn);
  cout << "Printing best JAWS duration : " << t.duration() << "s" << endl;


  cout << "Overall duration : " << globalStart.duration() << "s" << endl;
  return 0;
}
