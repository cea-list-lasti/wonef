
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
  time_t start = time(NULL);

  SimSynModule* simsyner = NULL;
  HyperHypoModule* hyperhypoer= NULL;
  MeroHoloModule* meroholoer= NULL;
  MeroHoloLikeHyperModule* merohololiker= NULL;
  LastChanceModule* lastchancer= NULL;

  cout << "Init duration : " << time(NULL) - start << " s " << endl;

  bool nounonly = false;
  bool noen = false;

  string suffix = "";
  string datafile = DATA_NOUN;

  vector<int> seq;
  for (int i = 1; i < argc ; i++) {
    cerr << "ARGV : '" << argv[i] << "'"<<endl;
    if (argv[i][0]=='n') {
      nounonly = true;
      suffix += ".noun";
    } else if (argv[i][0]=='E') {
      datafile = DATA_NOUN15;
      suffix += ".ewn";
    } else if (argv[i][0]=='W') {
      datafile = DATA_NOUN20;
      suffix += ".wolf";
    } else if (argv[i][0]=='1') {
      suffix += "1";
      seq.push_back(1);
    } else if (argv[i][0]=='2') {
      suffix += "2";
      seq.push_back(2);
    } else if (argv[i][0]=='3') {
      suffix += "3";
      seq.push_back(3);
    } else if (argv[i][0]=='4') {
      suffix += "4";
      seq.push_back(4);
    } else if (argv[i][0]=='5') {
      suffix += "5";
      seq.push_back(5);
    } else if (argv[i][0]=='6') {
      suffix += "6";
      seq.push_back(6);
    } else if (argv[i][0]=='7') {
      suffix += "7";
      seq.push_back(7);
    }  else if (argv[i][0]=='8') {
      suffix += "8";
      seq.push_back(8);
    }  else if (argv[i][0]=='9') {
      suffix += "9";
      seq.push_back(9);
    } else if (argv[i][0]=='N') {
      noen = true;
      stringstream ss ;
      ss << "." << argv[i];
      suffix += ss.str();
    }else {
      stringstream ss ;
      ss << "." << argv[i];
      suffix += ss.str();
    }
  }


  cout << "Init " << suffix << endl;
  start = time(NULL);
  set<string> dicfiles;
  dicfiles.insert(DICFILE);
  dicfiles.insert(DICFILE2);
  LoaderModule loader(datafile, dicfiles, NOUNS_LIST, nounonly,noen );
  WORDNET::WordNet wn = loader.load(false, -1); // verbose false
  cout << "Init duration : " << time(NULL) - start << " s " << endl;

  int nIteration = 0;
  for(vector<int>::iterator idModuleConf = seq.begin(); idModuleConf!= seq.end(); idModuleConf++) {
    nIteration++;
    switch (*idModuleConf) {
    case 1:
      simsyner = new SimSynModule(*idModuleConf, nIteration);

      cout << "First step "  << endl;
      if (wn["00472185"].frenchCandidates.size()!=0) {
          cerr << "TEST2 : " << wn["00472185"].frenchCandidates["coupling"].cand.size() << endl;
      }
      simsyner->process(wn);
      cout << "First step duration : " << time(NULL) - start << " s " << endl;
      delete simsyner;
      break;

    case 2 :
      hyperhypoer = new HyperHypoModule(datafile, "COMPDUNOM", R_HYPER, *idModuleConf, nIteration);
      cout << "Second step "  << endl;
      start = time(NULL);
      hyperhypoer->process(wn);
      cout << "Second step duration : " << time(NULL) - start << " s " << endl;
      delete hyperhypoer;
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

      merohololiker = new MeroHoloLikeHyperModule(datafile, *idModuleConf, nIteration);
      cout << "Third step " << endl;
      start = time(NULL);
      merohololiker->process(wn, R_HYPER, false);
      cout << "Third step duration : " << time(NULL) - start << " s " << endl;
      delete merohololiker;
      break;

    case 4 :
      meroholoer = new MeroHoloModule(datafile, *idModuleConf, nIteration);
      cout << "Fourth step " << endl;
      start = time(NULL);
      meroholoer->process(wn);
      //      meroholoer->finalize();
      cout << "Fourth step duration : " << time(NULL) - start << " s " << endl;
      delete meroholoer;
      break;

    case 5 :
      lastchancer = new LastChanceModule(*idModuleConf, nIteration);
      cout << "Fifth step " << endl;
      start = time(NULL);
      lastchancer->process(wn);
      cout << "Fifth step duration : " << time(NULL) - start << " s " << endl;
      delete lastchancer;
      break;

    case 6 :
      hyperhypoer = new HyperHypoModule(datafile, "SUJ_V", R_HYPER, *idModuleConf, nIteration);
      cout << "Six step "  << endl;
      start = time(NULL);
      hyperhypoer->process(wn, false);
      cout << "Six step duration : " << time(NULL) - start << " s " << endl;
      delete hyperhypoer;
      break;

    case 7 :
      hyperhypoer = new HyperHypoModule(datafile, "COMPDUNOM", R_HYPO, *idModuleConf, nIteration);
      cout << "Seventh step "  << endl;
      start = time(NULL);
      hyperhypoer->process(wn, false);
      cout << "Seventh step duration : " << time(NULL) - start << " s " << endl;
      delete hyperhypoer;
      break;

    case 8 :
      hyperhypoer = new HyperHypoModule(datafile, "COD_V", R_HYPER, *idModuleConf, nIteration);
      cout << "Eigth step "  << endl;
      start = time(NULL);
      hyperhypoer->process(wn, false);
      cout << "Eigth step duration : " << time(NULL) - start << " s " << endl;
      delete hyperhypoer;
      break;

    case 9 :
      hyperhypoer = new HyperHypoModule(datafile, "window10", R_HYPER, *idModuleConf, nIteration);
      cout << "Ninth step "  << endl;
      start = time(NULL);
      hyperhypoer->process(wn, false);
      cout << "Ninth step duration : " << time(NULL) - start << " s " << endl;
      delete hyperhypoer;
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
