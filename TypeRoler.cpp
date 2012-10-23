#include "TypeRoler.hpp"

#include "repository.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <algorithm>
#include <cassert>
#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


using namespace std;

TypeRoler::TypeRoler() {
}

TypeRoler::TypeRoler(string _dataFile, string relation) : 
  dataFile(_dataFile),
  topSize(10),
  thresCut(0) {
  initializeDicMap(dicmap, WORDS_IDS);
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

    std::cout << "note: computing the typeroler cache, next run will be faster." << std::endl;

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
          string semanticMapWord = dicmap[currentIdent];
          int sumOccs = processLine(ss.str(), thresCut, repository[semanticMapWord]);
          numCoocs[semanticMapWord] = sumOccs;
        }
      }
    }
    idss.close();

    writeRepository(protofile, relation);
  }
}

TypeRoler::~TypeRoler() {
}

void TypeRoler::writeRepository(std::string protofile, std::string relation) {
  std::cout << "Writing protobuf " << protofile << "!" << std::endl;
  Repository repositoryFile;
  repositoryFile.set_relation(relation);

  for(std::map<std::string, std::vector<pair<ulong,int> > >::iterator it = repository.begin();
    it != repository.end(); ++it) {
    Repository::Word *word = repositoryFile.add_word();
    word->set_key(it->first);
    word->set_numcoocs(numCoocs[it->first]);
    for(unsigned int i = 0; i < it->second.size(); i++) {
      word->add_related(it->second[i].first);
      word->add_score(it->second[i].second);
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

  /* Since protocol buffers for HyperHypo can be very large (window10 is around
   * 550MB), we need to work-around Google's 64MB limit by doing more work than
   * ParseFromIStream */
  int fd = open(protofile.c_str(), O_RDONLY);
  google::protobuf::io::ZeroCopyInputStream* raw_input = new google::protobuf::io::FileInputStream(fd);
  google::protobuf::io::CodedInputStream* coded_input = new google::protobuf::io::CodedInputStream(raw_input);
  // limit is 600, warn at 550
  coded_input->SetTotalBytesLimit(600*1024*1024, 550*1024*1024);
  if (!repositoryFile.ParseFromCodedStream(coded_input)) {
    std::cerr << "Failed to parse " << protofile << std::endl;
    exit(1);
  }

  assert(repositoryFile.relation() == relation);
  for(int i = 0; i < repositoryFile.word_size(); i++) {
    Repository::Word w = repositoryFile.word(i);

    std::vector<pair<ulong,int> > related;
    related.reserve(w.related_size());
    assert(w.related_size() == w.score_size());
    for(int j = 0; j < w.related_size(); j++) {
      related.push_back(pair<ulong,int>(w.related(j), w.score(j)));
    }

    repository[w.key()] = related;
    numCoocs[w.key()] = w.numcoocs();
  }
}

int TypeRoler::processLine(string s, float /*thresCut*/, vector<pair<ulong,int> >& ctxt) {
  stringstream ss;
  ss << s;
  ulong currentIdent;
  int nbOccs = 0, sumOccs = 0;
  while (ss.get() != '{');
  while (ss.get()!='}') {
    ss.unget();
    ss >> currentIdent;
    while (ss.get() != ',');
    ss >> nbOccs;
    sumOccs += nbOccs;
    ctxt.push_back(pair<ulong, int>(currentIdent, nbOccs));
    /* In TypeRoler::computeIsAScore, we assume those arrays are sorted, but
     * this only works if the underlying TYPEROLERFILE is sorted. Let's check
     * at runtime. */
    if (ctxt.size() > 1) {
        assert(currentIdent > ctxt[ctxt.size()-2].first);
    }
    while (ss.get() != ';');
  }
  // try to save memory since this barely holds in RAM for large syntactic relations
  ctxt.resize(ctxt.size());

  return sumOccs;
}

/* XXX Most of execution time is spent in this function: make sure to profile any change! */

/* This function computes the "strHyperonym is a hyperonym of strHyponym"
 *
 * It does so by looking at the shared syntaxic co-occurrences: strHyponym
 * should have the same cooccurences than strHypernym, but also more specific
 * ones.
 */
float TypeRoler::computeIsAScore(string strHyperonym, string strHyponym, TRMode mode) {
  /* If contexts are empty, simply return 0 */
  std::map<std::string, std::vector<std::pair<ulong, int> > >::iterator hyperonymCoocsIt, hyponymCoocsIt;

  hyperonymCoocsIt = repository.find(strHyperonym);
  hyponymCoocsIt = repository.find(strHyponym);

  if (hyponymCoocsIt == repository.end() || hyperonymCoocsIt == repository.end()) {
    return 0.0f;
  }

  /* Compute the intersection of syntaxic co-occurrence vectors */
  std::vector<std::pair<ulong,int> >& hyperonymCoocs = hyperonymCoocsIt->second;
  std::vector<std::pair<ulong,int> >& hyponymCoocs = hyponymCoocsIt->second;

  unsigned int i = 0, j = 0, interSize = 0;

  while (i < hyperonymCoocs.size() && j < hyponymCoocs.size()) {
    if (hyperonymCoocs[i].first < hyponymCoocs[j].first) {
      i++;
    } else if(hyperonymCoocs[i].first > hyponymCoocs[j].first) {
      j++;
    } else {
      /* Stick with what works best, not what looks best */
      // interSize += mode == R_HYPER ? hyperonymCoocs[i].second : hyponymCoocs[j].second;
      interSize++;
      i++;
      j++;
    }
  }

  /* normalize by the total number of cooccurrences */
  float normalized = interSize;

  switch(mode) {
    case R_HYPER:
      normalized /= repository[strHyponym].size();
      //normalized /= numCoocs[strHyponym];
      break;
    case R_HYPO:
      normalized /= repository[strHyperonym].size();
      //normalized /= numCoocs[strHyperonym];
      break;
    default:
      // fail early
      std::cerr << "TyperRoler::computeIsAScore wrong mode: " << mode << std::endl;
      exit(1);
      break;
  }

  return normalized;
}

