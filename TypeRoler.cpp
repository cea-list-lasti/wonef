#include "TypeRoler.hpp"

#include "repository.pb.h"

#include <algorithm>
#include <cassert>
#include <fstream>

using namespace std;

TypeRoler::TypeRoler() {
}

TypeRoler::TypeRoler(string _dataFile, string relation) : 
  dataFile(_dataFile),
  conv(CONVFILE, 67937),
  topSize(10),
  thresCut(0) {
  initializeDicMap(dicmap);
  initializeDicMapReverse(dicmapReverse, WORDS_IDS, false);

  std::string protofile = "/tmp/protobuf/" + relation;
  if (ifstream(protofile.c_str(), std::ios::in).good()) {
    readRepository(protofile, relation);
  } else {
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
        if (dicmap.find(currentIdent) == dicmap.end()) {
          std::cout << relation << ": " << currentIdent << " was not found in word.ids! Ignoring it." << std::endl;
        } else {
          processLine(ss.str(), thresCut, repository[dicmap[currentIdent]]);
        }
      }
    }
    idss.close();
    cout << "end load repository" << endl;

    writeRepository(protofile, relation);
  }
}

TypeRoler::~TypeRoler() {
}

void TypeRoler::writeRepository(std::string protofile, std::string relation) {
  std::cout << "Writing protobuf " << protofile << "!" << std::endl;
  Repository repositoryFile;
  repositoryFile.set_relation(relation);

  for(std::map<std::string, std::vector<ulong> >::iterator it = repository.begin();
    it != repository.end(); ++it) {
    Repository::Word *word = repositoryFile.add_word();
    word->set_key(it->first);
    for(unsigned int i = 0; i < it->second.size(); i++) {
      word->add_related(it->second[i]);
    }
  }

  fstream out(protofile.c_str(), ios::out | ios::binary | ios::trunc);
  repositoryFile.SerializeToOstream(&out);
  out.close();
}

void TypeRoler::readRepository(std::string protofile, std::string relation) {
  std::cout << "Reading protobuf " << protofile << "!" << std::endl;
  Repository repositoryFile;
  fstream in(protofile.c_str(), ios::in | ios::binary);
  if (!repositoryFile.ParseFromIstream(&in)) {
    std::cerr << "Failed to parse " << protofile << std::endl;
    exit(1);
  }

  assert(repositoryFile.relation() == relation);
  for(int i = 0; i < repositoryFile.word_size(); i++) {
    Repository::Word w = repositoryFile.word(i);

    std::vector<ulong> related;
    for(int j = 0; j < w.related_size(); j++) {
      related.push_back(w.related(j));
    }

    repository[w.key()] = related;
  }
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
    /* In TypeRoler::computeIsAScore, we assume those arrays are sorted, but
     * this only works if the underlying TYPEROLERFILE is sorted. Let's check
     * at runtime. */
    if (ctxt.size() > 1) {
        assert(currentIdent > ctxt[ctxt.size()-2]);
    }
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

