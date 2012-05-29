#include "../src/knn_search.h"
//#include "../src/converter.h"
#include "Paths.hpp"
#include "LoaderVerbs.hpp"
#include "Dumper.hpp"
#include "SimSynVerbsModule.hpp"
#include "HyperHypoModule.hpp"
#include "MeroHoloModule.hpp"
#include "MeroHoloLikeHyperModule.hpp"
#include "LastChanceModule.hpp"
#include <set>

using namespace std;


int main(int argc, char **argv) {

  time_t globalStart = time(NULL);
  time_t start = time(NULL);

  SimSynVerbsModule* simsyner = NULL;
  LastChanceModule* lastchancer= NULL;

  cout << "Init duration : " << time(NULL) - start << " s " << endl;

  string datafile = DATA_VERB;
  string suffix = "";
  vector<int> seq;
  for (int i = 1; i < argc ; i++) {
    cerr << "ARGV : '" << argv[i] << "'"<<endl;
    if (argv[i][0]=='1') { /*SimSynVerbsModule		S  */
      suffix += "1";
      seq.push_back(1);
    } else if (argv[i][0]=='5') { /*LastChanceModule	L  */
      suffix += "5";
      seq.push_back(5);
    } else {
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

  LoaderVerbsModule loader(datafile, dicfiles, VERBS_P_LIST);
  WORDNET::WordNet wn = loader.load(true, -1); // verbose true
  cout << "Init duration : " << time(NULL) - start << " s " << endl;

  for(vector<int>::iterator itseq = seq.begin(); itseq!= seq.end(); itseq++) {
    switch (*itseq) {
    case 1:
      simsyner = new SimSynVerbsModule;
      cout << "First step "  << endl;
      simsyner->process(wn);
      cout << "First step duration : " << time(NULL) - start << " s " << endl;
      delete simsyner;
      break;

    case 5 :
      lastchancer = new LastChanceModule;
      cout << "Fifth step " << endl;
      start = time(NULL);
      lastchancer->process(wn);
      cout << "Fifth step duration : " << time(NULL) - start << " s " << endl;
      delete lastchancer;
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


  cout << "Overall duration : " << time(NULL) - globalStart << " s " << endl;
  return 0;
}
