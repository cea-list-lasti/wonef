#include "Options.hpp"
#include "Paths.hpp"
#include "Loader.hpp"
#include "DEBVisDicDumper.hpp"
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



int main(int argc, char **argv) {

  Timer globalTimer;

  SimSynModule* simsyner = NULL;
  HyperHypoModule* hyperhypoer= NULL;
  LastChanceModule* lastchancer= NULL;

  std::string pos = "verb";
  WORDNET::pos = WORDNET::POS_of_string[pos];
  Options options(pos, argc, argv);

  std::cout << "Init " << options.suffix << std::endl;

  Timer t;
  std::cout << "Loading... ";
  LoaderModule loader(options.datafile, VERBS_P_LIST, pos);
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
      t.start();
      lastchancer = new LastChanceModule(idModuleConf, nIteration);
      lastchancer->process(wn);
      delete lastchancer;
      std::cout << "First step duration : " << t.duration() << "s" << std::endl;
      break;

    case 2 :
      t.start();
      simsyner = new SimSynModule(pos, idModuleConf, nIteration);
      simsyner->process(wn);
      delete simsyner;
      std::cout << "Second step duration : " << t.duration() << "s" << std::endl;
      break;

    case 3 :
      t.start();
      hyperhypoer = new HyperHypoModule(options.datafile, "SUJ_V_RELG.reverse", R_HYPER, pos, idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      std::cout << "Third step duration : " << t.duration() << "s" << std::endl;
      break;

    case 4 :
      t.start();
      hyperhypoer = new HyperHypoModule(options.datafile, "COD_V.reverse", R_HYPER, pos, idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      std::cout << "Fourth step duration : " << t.duration() << "s" << std::endl;
      break;

    case 5 :
      t.start();
      hyperhypoer = new HyperHypoModule(options.datafile, "CPL_V.reverse", R_HYPER, pos, idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      std::cout << "Fifth step duration : " << t.duration() << "s" << std::endl;
      break;

    case 6 :
      t.start();
      hyperhypoer = new HyperHypoModule(options.datafile, "ATB_S.reverse", R_HYPER, pos, idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      std::cout << "Six step duration : " << t.duration() << "s" << std::endl;
      break;

    case 7 :
      t.start();
      hyperhypoer = new HyperHypoModule(options.datafile, "SUJ_V.reverse", R_HYPER, pos, idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      std::cout << "Seventh step duration : " << t.duration() << "s" << std::endl;
      break;

    case 8 :
      t.start();
      hyperhypoer = new HyperHypoModule(options.datafile, "CPLV_V.reverse", R_HYPER, pos, idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      std::cout << "Eigth step duration : " << t.duration() << "s" << std::endl;
      break;

    default:
      break;
    }
  }

  std::cout << "Print Index  " << std::endl;
  DumperModule("data/jaws.verb" + options.suffix + ".xml").dump(wn);
  DEBVisDicDumperModule("data/jaws.verb" + options.suffix + ".deb.xml").dump(wn);
  std::cout << "Print index duration : " << t.duration() << "s" << std::endl;

  std::cout << "Choose best translations" << std::endl;
  BestTranslations(options).choose(wn);
  std::cout << "Choice duration : " << t.duration() << "s" << std::endl;

  std::cout << "Print best JAWS" << std::endl;
  DumperModule("data/jaws.best.verb" + options.suffix + ".xml").dump(wn);
  DEBVisDicDumperModule("data/jaws.best.verb" + options.suffix + ".deb.xml").dump(wn);
  std::cout << "Printing best JAWS duration : " << t.duration() << "s" << std::endl;


  std::cout << "Overall duration : " << globalTimer.duration() << "s" << std::endl;
  return 0;
}
