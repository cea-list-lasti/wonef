#include "Paths.hpp"
#include "Options.hpp"
#include "Loader.hpp"
#include "Dumper.hpp"
#include "DEBVisDicDumper.hpp"
#include "Timer.hpp"

#include "SimSynModule.hpp"
#include "BestTranslations.hpp"

#include <set>



int main(int argc, char **argv) {

  Timer globalStart;

  std::string pos = "adv";
  WORDNET::pos = WORDNET::POS_of_string[pos];
  Options options(pos, argc, argv);

  std::cout << "Init " << options.suffix << std::endl;

  Timer t;
  std::cout << "Loading... ";
  LoaderModule loader(options.datafile, ADVS_LIST, pos);
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
      std::cout << "First step "  << std::endl;
      t.start();
      SimSynModule(pos, idModuleConf, nIteration).process(wn);
      std::cout << "First step duration : " << t.duration() << "s" << std::endl;
      break;

    default:
      break;
    }

  }

  std::cout << "Dumping Jaws...";
  DumperModule("data/jaws.adv" + options.suffix + ".xml").dump(wn);
  DEBVisDicDumperModule("data/jaws.adv" + options.suffix + ".deb.xml").dump(wn);
  std::cout << t.duration() << "s" << std::endl;

  std::cout << "Choosing best translations...";
  BestTranslations(options).choose(wn);
  std::cout << t.duration() << "s" << std::endl;

  std::cout << "Print best JAWS" << std::endl;
  DumperModule("data/jaws.best.adv" + options.suffix + ".xml").dump(wn);
  DEBVisDicDumperModule("data/jaws.best.adv" + options.suffix + ".deb.xml").dump(wn);
  std::cout << "Printing best JAWS duration : " << t.duration() << "s" << std::endl;


  std::cout << "Overall duration : " << globalStart.duration() << "s" << std::endl;
  return 0;
}
