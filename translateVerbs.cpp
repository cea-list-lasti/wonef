#include "../src/knn_search.h"
//#include "../src/converter.h"
#include "Paths.hpp"
#include "LoaderVerbs.hpp"
#include "Dumper.hpp"
#include "SimSynVerbsModule.hpp"
#include "HyperHypoVerbsModule.hpp"
#include "MeroHoloModule.hpp"
#include "MeroHoloLikeHyperModule.hpp"
#include "LastChanceModule.hpp"
#include "BestTranslations.hpp"
#include <set>

using namespace std;


int main(int argc, char **argv) {

  time_t globalStart = time(NULL);

  SimSynVerbsModule* simsyner = NULL;
  HyperHypoVerbsModule* hyperhypoer= NULL;
  LastChanceModule* lastchancer= NULL;

  string datafile = DATA_VERB;
  string suffix = "";
  vector<int> seq;
  for (int i = 1; i < argc ; i++) {
    cerr << "ARGV : '" << argv[i] << "'"<<endl;
    if (argv[i][0]=='E') {
      datafile = DATA_VERB15;
      suffix += ".ewn";
    } else if (argv[i][0]=='1') { /*LastChanceModule			*/
      suffix += "1";
      seq.push_back(1);
    } else if (argv[i][0]=='2') { /*SimSynVerbsModule			*/
      suffix += "2";
      seq.push_back(2);
    } else if (argv[i][0]=='3') { /*HyperHypoModule SUJ_V_RELG.reverse	*/
      suffix += "3";
      seq.push_back(3);
    } else if (argv[i][0]=='4') { /*HyperHypoModule COD_V.reverse	*/
      suffix += "4";
      seq.push_back(4);
    } else if (argv[i][0]=='5') { /*HyperHypoModule CPL_V.reverse	*/
      suffix += "5";
      seq.push_back(5);
    } else if (argv[i][0]=='6') { /*HyperHypoModule ATB_S.reverse	*/
      suffix += "6";
      seq.push_back(6);
    } else if (argv[i][0]=='7') { /*HyperHypoModule SUJ_V.reverse	*/
      suffix += "7";
      seq.push_back(7);
    } else if (argv[i][0]=='8') { /*HyperHypoModule CPLV_V.reverse	*/
      suffix += "8";
      seq.push_back(8);
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

  LoaderVerbsModule loader(datafile, dicfiles, VERBS_P_LIST);
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
      simsyner = new SimSynVerbsModule(*idModuleConf, nIteration);
      simsyner->process(wn);
      cout << "Second step duration : " << time(NULL) - start << " s " << endl;
      delete simsyner;
      break;

    case 3 :
      cout << "Third step "  << endl;
      start = time(NULL);
      hyperhypoer = new HyperHypoVerbsModule(datafile, "SUJ_V_RELG.reverse", R_HYPER, *idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Third step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 4 :
      cout << "Fourth step "  << endl;
      start = time(NULL);
      hyperhypoer = new HyperHypoVerbsModule(datafile, "COD_V.reverse", R_HYPER, *idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Fourth step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 5 :
      cout << "Fifth step "  << endl;
      start = time(NULL);
      hyperhypoer = new HyperHypoVerbsModule(datafile, "CPL_V.reverse", R_HYPER, *idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Fifth step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 6 :
      cout << "Six step "  << endl;
      start = time(NULL);
      hyperhypoer = new HyperHypoVerbsModule(datafile, "ATB_S.reverse", R_HYPER, *idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Six step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 7 :
      cout << "Seventh step "  << endl;
      start = time(NULL);
      hyperhypoer = new HyperHypoVerbsModule(datafile, "SUJ_V.reverse", R_HYPER, *idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Seventh step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 8 :
      cout << "Eigth step "  << endl;
      start = time(NULL);
      hyperhypoer = new HyperHypoVerbsModule(datafile, "CPLV_V.reverse", R_HYPER, *idModuleConf, nIteration);
      hyperhypoer->process(wn);
      delete hyperhypoer;
      cout << "Eigth step duration : " << time(NULL) - start << " s " << endl;
      break;

    default:
      break;
    }
  }

  cout << "Print Index  " << endl;
  start = time(NULL);
  DumperModule dumper("data2/data.fr.verbs" + suffix, "data2/index.fr.verbs" + suffix);
  dumper.dump(wn);
  cout << "Print index duration : " << time(NULL) - start << " s " << endl;

  cout << "Choose best translations" << endl;
  start = time(NULL);
  BestTranslations bestTranslations;
  bestTranslations.choose(wn);
  cout << "Choice duration : " << time(NULL) - start << " s " << endl;

  cout << "Print best JAWS" << endl;
  start = time(NULL);
  DumperModule dumperBest("data2/data.fr.verbs.best" + suffix, "data2/index.fr.verbs.best" + suffix);
  dumperBest.dump(wn);
  cout << "Printing best JAWS duration : " << time(NULL) - start << " s " << endl;


  cout << "Overall duration : " << time(NULL) - globalStart << " s " << endl;
  return 0;
}
