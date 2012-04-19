#include <string>
#include "WordNet.hpp"

using namespace std;

class DumperModule {
  public : 
  DumperModule(string datafile, string indexfile) ;
  ~DumperModule() ;


  void dump(WORDNET::WordNet& wn, bool verbose=false) ;

  private :
  string datafile;
  string indexfile;
  map<string, set <string> > indexTgt;
  //  void fillIndex(WORDNET::WordNet& wn);

  void printIndex(bool verbose=false);
  void printData(WORDNET::WordNet& wn, bool verbose=false);
  void printUnsolved(WORDNET::WordNet& wn);


};
