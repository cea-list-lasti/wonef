#include "../src/knn_search.h"
//#include "../src/converter.h"
#include "Paths.hpp"
#include "Options.hpp"
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
  LastChanceModule* lastchancer= NULL;

  string pos = "adj";
  Options options(pos, argc, argv);

  cout << "Init " << options.suffix << endl;
  time_t start = time(NULL);

  set<string> dicfiles{DICFILE, DICFILE2};

  LoaderModule loader(options.datafile, dicfiles, ADJS_LIST, pos, options.noen);
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

    default:
      break;
    }

  }

  cout << "Print Index  " << endl;
  start = time(NULL);
  DumperModule dumper("data2/data.fr.adjs" + options.suffix, "data2/index.fr.adjs" + options.suffix);
  dumper.dump(wn);
  cout << "Print index duration : " << time(NULL) - start << " s " << endl;

  cout << "Choose best translations" << endl;
  start = time(NULL);
  BestTranslations bestTranslations;
  bestTranslations.choose(wn);
  cout << "Choice duration : " << time(NULL) - start << " s " << endl;

  cout << "Print best JAWS" << endl;
  start = time(NULL);
  DumperModule dumperBest("data2/data.fr.adjs.best" + options.suffix, "data2/index.fr.adjs.best" + options.suffix);
  dumperBest.dump(wn);
  cout << "Printing best JAWS duration : " << time(NULL) - start << " s " << endl;


  cout << "Overall duration : " << time(NULL) - globalStart << " s " << endl;
  return 0;
}
