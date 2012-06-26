#include "TypeRoler.hpp"
 
#include <algorithm>

using namespace std;

TypeRoler::TypeRoler() {
}

TypeRoler::TypeRoler(string _dataFile) : 
  dataFile(_dataFile),
  conv(CONVFILE, 67937),
  topSize(10),
  thresCut(0) {
  initializeDicMap(dicmap);  
  initializeDicMapReverse(dicmapReverse, WORDS_IDS, false);
  ifstream idss(dataFile.c_str());
  if (idss.fail()) {
    cerr << "Oops, " << dataFile << " doesn't exist. " << __FILE__ << ":" << __LINE__ << endl;
    exit(-1);
  }
  string s;
  while (getline(idss, s) ) {
    stringstream ss;
    ss <<s;
    ulong currentIdent;
    ss>>currentIdent;
    if (s.find('}') - s.find('{') !=1 && s.find(']')-s.find('[')!=3) {
      processLine(ss.str(), thresCut, repository[dicmap[currentIdent]]);
    }
  }
  idss.close();  
  cout << "end load repository" << endl;
}

TypeRoler::~TypeRoler() {
}



void TypeRoler::processLine(string s, float /*thresCut*/, vector<ulong>& ctxt) {  
  stringstream ss;
  ss << s;
  ulong currentIdent;
  float nbOccs = 0;
  while (ss.get() != '{');
  while (ss.get()!='}') {
    ss.unget();
    ss >> currentIdent;
    while (ss.get() != ',');
    ss >> nbOccs;
    ctxt.push_back(currentIdent);
    while (ss.get() != ';');
  }
}




float TypeRoler::computeIsAScore( string strA, string strB, TRMode mode) {
  std::vector<ulong> tmpA = repository[strA];
  std::vector<ulong> tmpB = repository[strB];

  std::vector<ulong> intersection;
  set_intersection(tmpA.begin(), tmpA.end(), tmpB.begin(), tmpB.end(), inserter(intersection, intersection.end()));

  float xIsY = (float)intersection.size();
  if (mode==R_HYPER) {
    xIsY/=tmpB.size();
  } else if (mode==R_HYPO) {
    xIsY/=tmpA.size();
  }

  return xIsY;
}

