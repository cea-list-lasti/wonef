
#include "../knn_search.h"
//#include "../converter.h"
#include "Paths.hpp"
#include "Loader.hpp"
#include "Dumper.hpp"
#include "SimSynModule.hpp"
#include "HyperHypoModule.hpp"
#include "MeroHoloModule.hpp"
#include "MeroHoloLikeHyperModule.hpp"
#include "LastChanceModule.hpp"
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
  

  cout << "Init "  << endl;
  start = time(NULL);
  set<string> dicfiles;
  dicfiles.insert(DICFILE);
  dicfiles.insert(DICFILE2);
  LoaderModule loader(datafile, dicfiles, NOUNS_LIST, nounonly,noen );
  WORDNET::WordNet wn = loader.load(false, -1); // verbose false
  cout << "Init duration : " << time(NULL) - start << " s " << endl;

  for(vector<int>::iterator itseq = seq.begin(); itseq!= seq.end(); itseq++) {
    switch (*itseq) {
    case 1:
      if (simsyner == NULL) {
	simsyner = new SimSynModule;
      }

      cout << "First step "  << endl;
      if (wn["00472185"].frenchCandidates.size()!=0) {
	cerr << "TEST2 : " << wn["00472185"].frenchCandidates["coupling"].cand.size() << endl;
      }
      simsyner->process(wn);
      cout << "First step duration : " << time(NULL) - start << " s " << endl;
      break;
      
    case 2 : 
      delete merohololiker;    
      delete meroholoer;
      delete hyperhypoer;
      hyperhypoer = new HyperHypoModule(datafile, "COMPDUNOM");

      cout << "Second step "  << endl;
      start = time(NULL);
      hyperhypoer->process(wn, R_HYPER);
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


      if (merohololiker == NULL) {	
	delete hyperhypoer;
	delete meroholoer;
	merohololiker = new MeroHoloLikeHyperModule(datafile);
      }
      cout << "Third step " << endl;
      start = time(NULL);
      merohololiker->process(wn, R_HYPER, false);
      cout << "Third step duration : " << time(NULL) - start << " s " << endl;
      break;


      break;

    case 4 :      

      if (meroholoer == NULL) {
	delete hyperhypoer;
	delete merohololiker;	
	meroholoer = new MeroHoloModule(datafile);
      }
      cout << "Fourth step " << endl;
      start = time(NULL);
      meroholoer->process(wn);
      //      meroholoer->finalize();
      cout << "Fourth step duration : " << time(NULL) - start << " s " << endl;
      break;
  
    case 5 : 

      if (lastchancer == NULL) {
	lastchancer = new LastChanceModule;
      }
      cout << "Fifth step " << endl;
      start = time(NULL);
      lastchancer->process(wn);
      cout << "Fifth step duration : " << time(NULL) - start << " s " << endl;
      break;


      
    case 6 : 

      delete merohololiker;
      delete hyperhypoer;
      hyperhypoer = new HyperHypoModule(datafile, "SUJ_V");

      cout << "Six step "  << endl;
      start = time(NULL);
      hyperhypoer->process(wn, R_HYPER, false);
      cout << "Six step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 7 : 
      delete merohololiker;
      delete hyperhypoer;
      hyperhypoer = new HyperHypoModule(datafile, "COMPDUNOM");
      cout << "Seventh step "  << endl;
      start = time(NULL);
      hyperhypoer->process(wn, R_HYPO, false);
      cout << "Seventh step duration : " << time(NULL) - start << " s " << endl;
      
    case 8 : 


      delete hyperhypoer;
      hyperhypoer = new HyperHypoModule(datafile, "COD_V");

      cout << "Six step "  << endl;
      start = time(NULL);
      hyperhypoer->process(wn, R_HYPER, false);
      cout << "Six step duration : " << time(NULL) - start << " s " << endl;
      break;

    case 9 : 


      delete hyperhypoer;
      hyperhypoer = new HyperHypoModule(datafile, "window10");

      cout << "Six step "  << endl;
      start = time(NULL);
      hyperhypoer->process(wn, R_HYPER, false);
      cout << "Six step duration : " << time(NULL) - start << " s " << endl;
      break;
      

    default:
      break;
    }

  }
  
  cout << "Print Index  " << endl;
  start = time(NULL);
  DumperModule dumper("data2/data.fr.nouns" + suffix, "data2/index.fr.noun" + suffix);
  dumper.dump(wn, true);
  cout << "Print index duration : " << time(NULL) - start << " s " << endl;


  cout << "Overall duration : " << time(NULL) - globalStart << " s " << endl;
  return 0;
}
