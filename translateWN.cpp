
#include "../src/knn_search.h"
//#include "../src/converter.h"
#include "Paths.hpp"
#include "Loader.hpp"
#include "Dumper.hpp"
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

  bool noen = false;

  string pos = "noun";
  string suffix = "";
  string datafile = DATA_NOUN;

  vector<int> seq;
  for (int i = 1; i < argc ; i++) {
    if (argv[i][0]=='E') {
      datafile = DATA_NOUN15;
      suffix += ".ewn";
    } else if (argv[i][0]=='W') {
      datafile = DATA_NOUN20;
      suffix += ".wolf";
    /* simple integer: add the corresponding module */
    } else if (argv[i][0] >= '1' && argv[i][0] <= '9') {
      suffix += argv[i][0];
      seq.push_back(argv[i][0] - '0');
    } else if (argv[i][0]=='N') {
      noen = true;
      stringstream ss ;
      ss << "." << argv[i];
      suffix += ss.str();
    } else {
      stringstream ss ;
      ss << "." << argv[i];
      suffix += ss.str();
    }
  }


  cout << "Init " << suffix << endl;
  time_t start = time(NULL);

  set<string> dicfiles;
  dicfiles.insert(DICFILE);
  dicfiles.insert(DICFILE2);

  LoaderModule loader(datafile, dicfiles, NOUNS_LIST, pos, noen);
  WORDNET::WordNet wn = loader.load(false, -1); // verbose false
  cout << "Init duration : " << time(NULL) - start << " s " << endl;

  int nIteration = 0;
  for(vector<int>::iterator idModuleConf = seq.begin(); idModuleConf!= seq.end(); idModuleConf++) {
    nIteration++;
    switch (*idModuleConf) {
    case 1:
      cout << "First step "  << endl;
      start = time(NULL);
      simsyner = new SimSynModule(pos, *idModuleConf, nIteration);
      simsyner->process(wn);
      delete simsyner;
      cout << "First step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 2 :
      cout << "Second step "  << endl;
      start = time(NULL);
      hyperhypoer = new HyperHypoModule(datafile, "COMPDUNOM", R_HYPER, pos, *idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Second step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 3 :
      /*
      if (hyperhypoer == NULL) {
	hyperhypoer = new HyperHypoModule(datafile);
      }
      cout << "Third step "  << endl;
      start = time(NULL);
      hyperhypoer->process(wn, R_HYPO, true);
      cout << "Third step duration : " << time(NULL) - start << " s " << endl;
      */

      cout << "Third step " << endl;
      start = time(NULL);
      merohololiker = new MeroHoloLikeHyperModule(datafile, *idModuleConf, nIteration);
      merohololiker->process(wn, R_HYPER, false);
      delete merohololiker;
      cout << "Third step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 4 :
      cout << "Fourth step " << endl;
      start = time(NULL);
      meroholoer = new MeroHoloModule(datafile, *idModuleConf, nIteration);
      meroholoer->process(wn);
      //      meroholoer->finalize();
      delete meroholoer;
      cout << "Fourth step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 5 :
      cout << "Fifth step " << endl;
      start = time(NULL);
      lastchancer = new LastChanceModule(*idModuleConf, nIteration);
      lastchancer->process(wn);
      delete lastchancer;
      cout << "Fifth step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 6 :
      cout << "Six step "  << endl;
      start = time(NULL);
      hyperhypoer = new HyperHypoModule(datafile, "SUJ_V", R_HYPER, pos, *idModuleConf, nIteration);
      hyperhypoer->process(wn, false);
      delete hyperhypoer;
      cout << "Six step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 7 :
      cout << "Seventh step "  << endl;
      hyperhypoer = new HyperHypoModule(datafile, "COMPDUNOM", R_HYPO, pos, *idModuleConf, nIteration);
      start = time(NULL);
      hyperhypoer->process(wn, false);
      delete hyperhypoer;
      cout << "Seventh step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 8 :
      cout << "Eigth step "  << endl;
      start = time(NULL);
      hyperhypoer = new HyperHypoModule(datafile, "COD_V", R_HYPER, pos, *idModuleConf, nIteration);
      hyperhypoer->process(wn, false);
      delete hyperhypoer;
      cout << "Eigth step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 9 :
      cout << "Ninth step "  << endl;
      start = time(NULL);
      hyperhypoer = new HyperHypoModule(datafile, "window10", R_HYPER, pos, *idModuleConf, nIteration);
      hyperhypoer->process(wn, false);
      delete hyperhypoer;
      cout << "Ninth step duration : " << time(NULL) - start << " s " << endl;
      break;

    default:
      break;
    }

  }

  cout << "Print Index  " << endl;
  start = time(NULL);
  DumperModule dumper("data2/data.fr.nouns" + suffix, "data2/index.fr.noun" + suffix);
  dumper.dump(wn);
  cout << "Print index duration : " << time(NULL) - start << " s " << endl;

  cout << "Choose best translations" << endl;
  start = time(NULL);
  BestTranslations bestTranslations;
  bestTranslations.choose(wn);
  cout << "Choice duration : " << time(NULL) - start << " s " << endl;

  cout << "Print best JAWS" << endl;
  start = time(NULL);
  DumperModule dumperBest("data2/data.fr.nouns.best" + suffix, "data2/index.fr.nouns.best" + suffix);
  dumperBest.dump(wn);
  cout << "Printing best JAWS duration : " << time(NULL) - start << " s " << endl;


  cout << "Overall duration : " << time(NULL) - globalStart << " s " << endl;
  return 0;
}
