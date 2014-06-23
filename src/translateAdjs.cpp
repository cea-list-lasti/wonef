#include "Timer.hpp"
#include "Paths.hpp"
#include "Options.hpp"
#include "Loader.hpp"
#include "Dumper.hpp"
#include "DEBVisDicDumper.hpp"

#include "ExtractorModule.hpp"
#include "SimSynModule.hpp"
#include "HyperHypoModule.hpp"
#include "MeroHoloModule.hpp"
#include "MeroHoloLikeHyperModule.hpp"
#include "LastChanceModule.hpp"
#include "BestTranslations.hpp"

#include <set>



int main(int argc, char **argv) {

  Timer globalStart;

  SimSynModule* simsyner = NULL;
  LastChanceModule* lastchancer= NULL;

  std::string pos = "adj";
  WORDNET::pos = WORDNET::POS_of_string[pos];
  Options options(pos, argc, argv);

  std::cout << "Init " << options.suffix << std::endl;

  Timer t;
  std::cout << "Loading... ";
  LoaderModule loader(options.datafile, ADJS_LIST, pos);
  WORDNET::WordNet wn = loader.load();
  std::cout << t.duration() << "s" << std::endl;

  t.start();
  ExtractorModule extractor(pos, options);
  extractor.process(wn);
  std::cout << "Extraction duration : " << t.duration() << "s" << std::endl;

  int nIteration = 0;
  for(int idModuleConf: options.moduleSequence) {
    nIteration++;
    switch (idModuleConf) {
    case 1:
      std::cout << "First step " << std::endl;
      t.start();
      lastchancer = new LastChanceModule(idModuleConf, nIteration);
      lastchancer->process(wn);
      delete lastchancer;
      std::cout << "First step duration : " << t.duration() << "s" << std::endl;
      break;

    case 2 :
      std::cout << "Second step "  << std::endl;
      t.start();
      simsyner = new SimSynModule(pos, idModuleConf, nIteration);
      simsyner->process(wn);
      delete simsyner;
      std::cout << "Second step duration : " << t.duration() << "s" << std::endl;
      break;

    default:
      break;
    }

  }

  std::cout << "Dumping Jaws...";
  DumperModule("data/jaws.adj" + options.suffix + ".xml").dump(wn);
  DEBVisDicDumperModule("data/jaws.adj" + options.suffix + ".deb.xml").dump(wn);
  std::cout << t.duration() << "s" << std::endl;

  std::cout << "Choosing best translations...";
  BestTranslations(options).choose(wn);
  std::cout << t.duration() << "s" << std::endl;

  std::cout << "Print best JAWS" << std::endl;
  DumperModule("data/jaws.best.adj" + options.suffix + ".xml").dump(wn);
  DEBVisDicDumperModule("data/jaws.best.adj" + options.suffix + ".deb.xml").dump(wn);
  std::cout << "Printing best JAWS duration : " << t.duration() << "s" << std::endl;


  std::cout << "Overall duration : " << globalStart.duration() << "s" << std::endl;
  return 0;
}
