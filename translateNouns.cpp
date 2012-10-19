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
  HyperHypoModule* hyperhypoer= NULL;
  MeroHoloModule* meroholoer= NULL;
  MeroHoloLikeHyperModule* merohololiker= NULL;
  LastChanceModule* lastchancer= NULL;

  set<string> dicfiles{DICFILE, DICFILE2};
  string pos = "noun";
  Options options(pos, argc, argv);

  cout << "Init " << options.suffix << endl;

  time_t start = time(NULL);
  LoaderModule loader(options.datafile, dicfiles, NOUNS_LIST, pos);
  WORDNET::WordNet wn = loader.load(false, -1); // verbose false
  cout << "Loading duration : " << time(NULL) - start << " s " << endl;

  start = time(NULL);
  ExtractorModule extractor(pos, options.extractionSet);
  extractor.process(wn);
  cout << "Extraction duration : " << time(NULL) - start << " s " << endl;

  int nIteration = 0;
  for(int idModuleConf: options.moduleSequence) {
    nIteration++;
    switch (idModuleConf) {
    case 1:
      cout << "First step "  << endl;
      start = time(NULL);
      simsyner = new SimSynModule(pos, idModuleConf, nIteration);
      simsyner->process(wn);
      delete simsyner;
      cout << "First step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 2 :
      cout << "Second step "  << endl;
      start = time(NULL);
      hyperhypoer = new HyperHypoModule(options.datafile, "COMPDUNOM", R_HYPER, pos, idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Second step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 3 :
      cout << "Third step " << endl;
      start = time(NULL);
      merohololiker = new MeroHoloLikeHyperModule(options.datafile, idModuleConf, nIteration);
      merohololiker->process(wn, R_HYPER);
      delete merohololiker;
      cout << "Third step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 4 :
      cout << "Fourth step " << endl;
      start = time(NULL);
      meroholoer = new MeroHoloModule(options.datafile, idModuleConf, nIteration);
      meroholoer->process(wn);
      //      meroholoer->finalize();
      delete meroholoer;
      cout << "Fourth step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 5 :
      cout << "Fifth step " << endl;
      start = time(NULL);
      lastchancer = new LastChanceModule(idModuleConf, nIteration);
      lastchancer->process(wn);
      delete lastchancer;
      cout << "Fifth step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 6 :
      cout << "Six step "  << endl;
      start = time(NULL);
      hyperhypoer = new HyperHypoModule(options.datafile, "SUJ_V", R_HYPER, pos, idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Six step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 7 :
      cout << "Seventh step "  << endl;
      hyperhypoer = new HyperHypoModule(options.datafile, "COMPDUNOM", R_HYPO, pos, idModuleConf, nIteration);
      start = time(NULL);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Seventh step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 8 :
      cout << "Eigth step "  << endl;
      start = time(NULL);
      hyperhypoer = new HyperHypoModule(options.datafile, "COD_V", R_HYPER, pos, idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Eigth step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 9 :
      cout << "Ninth step "  << endl;
      start = time(NULL);
      hyperhypoer = new HyperHypoModule(options.datafile, "window10", R_HYPER, pos, idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Ninth step duration : " << time(NULL) - start << " s " << endl;
      break;

    default:
      break;
    }

  }

  cout << "Print Index  " << endl;
  start = time(NULL);
  DumperModule dumper("data2/data.fr.nouns" + options.suffix, "data2/index.fr.noun" + options.suffix);
  dumper.dump(wn);
  cout << "Print index duration : " << time(NULL) - start << " s " << endl;

  cout << "Choose best translations" << endl;
  start = time(NULL);
  BestTranslations bestTranslations;
  bestTranslations.choose(wn);
  cout << "Choice duration : " << time(NULL) - start << " s " << endl;

  cout << "Print best JAWS" << endl;
  start = time(NULL);
  DumperModule dumperBest("data2/data.fr.nouns.best" + options.suffix, "data2/index.fr.nouns.best" + options.suffix);
  dumperBest.dump(wn);
  cout << "Printing best JAWS duration : " << time(NULL) - start << " s " << endl;


  cout << "Overall duration : " << time(NULL) - globalStart << " s " << endl;
  return 0;
}
