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



int main(int argc, char **argv) {

  Timer globalTimer;

  std::string pos = "noun";
  WORDNET::pos = WORDNET::POS_of_string[pos];
  Options options(pos, argc, argv);

  std::cout << "Init " << options.suffix << std::endl;

  Timer t;
  LoaderModule loader(options.datafile, NOUNS_LIST, pos);
  WORDNET::WordNet wn = loader.load();
  std::cout << "Loading duration : " << t.duration() << "s" << std::endl;

  t.start();
  ExtractorModule extractor(pos, options);
  extractor.process(wn);
  std::cout << "Extraction duration : " << t.duration() << "s" << std::endl;


  int nIteration = 0;
  for(int idModuleConf: options.moduleSequence) {
    nIteration++;
    switch (idModuleConf) {
    case 1:
      t.start();
      SimSynModule(pos, idModuleConf, nIteration).process(wn);
      std::cout << "First step duration : " << t.duration() << "s" << std::endl;
      break;

    case 2 :
      t.start();
      HyperHypoModule(options.datafile, "COMPDUNOM", R_HYPER, pos, idModuleConf, nIteration).process(wn);
      std::cout << "Second step duration : " << t.duration() << "s" << std::endl;
      break;

    case 3 :
      t.start();
      MeroHoloLikeHyperModule(options.datafile, idModuleConf, nIteration).process(wn, R_HYPER);
      std::cout << "Third step duration : " << t.duration() << "s" << std::endl;
      break;

    case 4 :
      t.start();
      MeroHoloModule(options.datafile, idModuleConf, nIteration).process(wn);
      std::cout << "Fourth step duration : " << t.duration() << "s" << std::endl;
      break;

    case 6 :
      t.start();
      HyperHypoModule(options.datafile, "SUJ_V", R_HYPER, pos, idModuleConf, nIteration).process(wn);
      std::cout << "Six step duration : " << t.duration() << "s" << std::endl;
      break;

    case 7 :
      t.start();
      HyperHypoModule(options.datafile, "COMPDUNOM", R_HYPO, pos, idModuleConf, nIteration).process(wn);
      std::cout << "Seventh step duration : " << t.duration() << "s" << std::endl;
      break;

    case 8 :
      t.start();
      HyperHypoModule(options.datafile, "COD_V", R_HYPER, pos, idModuleConf, nIteration).process(wn);
      std::cout << "Eigth step duration : " << t.duration() << "s" << std::endl;
      break;

    case 9 :
      t.start();
      HyperHypoModule(options.datafile, "window10", R_HYPER, pos, idModuleConf, nIteration).process(wn);
      std::cout << "Ninth step duration : " << t.duration() << "s" << std::endl;
      break;

    default:
	  std::cerr << "Step " << idModuleConf << " does not exist!" << std::endl;
      exit(-1);
      break;
    }

  }

  DumperModule("data/jaws.noun" + options.suffix + ".xml").dump(wn);
  DEBVisDicDumperModule("data/jaws.noun" + options.suffix + ".deb.xml").dump(wn);
  std::cout << "Dump JAWS duration : " << t.duration() << "s" << std::endl;

  BestTranslations(options).choose(wn);
  std::cout << "BestTranslation duration : " << t.duration() << "s" << std::endl;

  DumperModule("data/jaws.best.noun" + options.suffix + ".xml").dump(wn);
  DEBVisDicDumperModule("data/jaws.best.noun" + options.suffix + ".deb.xml").dump(wn);
  std::cout << "Dump best JAWS duration : " << t.duration() << "s" << std::endl;


  std::cout << "Overall duration: " << globalTimer.duration() << "s" << std::endl;
  return 0;
}
