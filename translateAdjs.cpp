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
  LastChanceModule* lastchancer= NULL;

  bool noen = false;

  string pos = "adj";
  string suffix = "";
  string datafile = DATA_ADJ;

  vector<int> seq;
  for (int i = 1; i < argc ; i++) {
    if (argv[i][0]=='E') {
      datafile = DATA_ADJ15;
      suffix += ".ewn";
    /* simple integer: add the corresponding module */
    } else if (argv[i][0] >= '1' && argv[i][0] <= '2') {
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

  LoaderModule loader(datafile, dicfiles, ADJS_LIST, pos, noen);
  WORDNET::WordNet wn = loader.load(false, -1); // verbose false
  cout << "Init duration : " << time(NULL) - start << " s " << endl;

  int nIteration = 0;
  for(vector<int>::iterator idModuleConf = seq.begin(); idModuleConf!= seq.end(); idModuleConf++) {
    nIteration++;
    switch (*idModuleConf) {
    case 1:
      cout << "First step " << endl;
      start = time(NULL);
      lastchancer = new LastChanceModule(*idModuleConf, nIteration);
      lastchancer->process(wn);
      delete lastchancer;
      cout << "First step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 2 :
      cout << "Second step "  << endl;
      start = time(NULL);
      simsyner = new SimSynModule(pos, *idModuleConf, nIteration);
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
  DumperModule dumper("data2/data.fr.adjs" + suffix, "data2/index.fr.adjs" + suffix);
  dumper.dump(wn);
  cout << "Print index duration : " << time(NULL) - start << " s " << endl;

  cout << "Choose best translations" << endl;
  start = time(NULL);
  BestTranslations bestTranslations;
  bestTranslations.choose(wn);
  cout << "Choice duration : " << time(NULL) - start << " s " << endl;

  cout << "Print best JAWS" << endl;
  start = time(NULL);
  DumperModule dumperBest("data2/data.fr.adjs.best" + suffix, "data2/index.fr.adjs.best" + suffix);
  dumperBest.dump(wn);
  cout << "Printing best JAWS duration : " << time(NULL) - start << " s " << endl;


  cout << "Overall duration : " << time(NULL) - globalStart << " s " << endl;
  return 0;
}
