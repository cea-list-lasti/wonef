#include "TypeRoler.hpp"
 

using namespace std;

TypeRoler::TypeRoler() {
}

TypeRoler::TypeRoler(string _dataFile) : 
  dataFile(_dataFile),
  conv(CONVFILE, 67937),
  topSize(10),
  thresCut(0) {
  initializeDicMap(dicmap);  
  initializeDicMapReverse(dicmapReverse, false);    
  ifstream idss(dataFile.c_str());
  string s;
  cerr << "Loading " << dataFile << endl;
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



void TypeRoler::processLine(string s, float thresCut, map<ulong, float>& ctxt) {  
  stringstream ss;
  ss << s;
  ulong currentIdent;
  float nbOccs = 0;
  float sum = 0;
  while (ss.get()!='}') {
    ss.unget();
    ss >> currentIdent;
    ss.ignore(16, ',');
    ss >> nbOccs;
    ctxt[currentIdent]=nbOccs;
    ss.ignore(4, ';');
    sum+=nbOccs;
  }  

  for (map<ulong, float>::iterator it = ctxt.begin(); it!=ctxt.end() ; it++) {
    it->second/=sum;
  }
}




float TypeRoler::computeIsAScore( string strA, string strB, TRMode mode) {
  set<int> ownerIds;
  ownerIds.insert(dicmapReverse[strA]);
  ownerIds.insert(dicmapReverse[strB]);
  vector<pair<string, pair<float,float > > > bestInter;
  map<ulong, float> tmpmapA ;
  map<ulong, float> tmpmapB ;
  //  map<ulong, float> tmpmapUnion ;
  tmpmapA.insert( repository[strA].begin(),repository[strA].end()) ;
  //tmpmapUnion.insert( repository[strA].begin(),repository[strA].end()) ;
  tmpmapB.insert(  repository[strB].begin(),repository[strB].end()) ;
  //  tmpmapUnion.insert(repository[strB].begin(),repository[strB].end()) ;
  //TOFIX : B values erase former A value... :(  -> deprecated

  //  int unionSize = tmpmapUnion.size();
  int interSize = 0;
  
  float thres = 0;

  for ( map<ulong, float>::iterator it = tmpmapB.begin(); it!=tmpmapB.end() ;  it++) {
    if (tmpmapA.find(it->first)!=tmpmapA.end()) {
      interSize++;
      /*
      // seems to be obsolete
      float nbOccs = it->second + tmpmapA[it->first];
      pair<float, float> pairOccs = pair<float, float>(it->second, tmpmapA[it->first]);
      if (bestInter.size()<topSize) {     
	bestInter.push_back(pair<string, pair<float, float> >(dicmap[it->first], pairOccs));	
      } else if (bestInter.size()==topSize && nbOccs>thres) {
	bestInter.push_back(pair<string, pair<float, float> >(dicmap[it->first], pairOccs));
	sort(bestInter.begin(), bestInter.end(), cmpInter);	  
	bestInter.pop_back();
	thres = (bestInter.end()-1)->second.first  +(bestInter.end()-1)->second.second ;
      }
      //*/
    }
  }

  float xIsY = (float)interSize;
    if (mode==R_HYPER) {
      xIsY/=tmpmapB.size();
    } else if (mode==R_HYPO) {
      xIsY/=tmpmapA.size();
    }
  //  cerr << strA << "("<<tmpmapA.size()<<") is " << strB << "("<<tmpmapB.size()<<") : " <<  interSize  << " -> " << xIsY << endl;

  return xIsY;
}

bool TypeRoler::cmpInter(pair<string, pair<float, float> > a, pair<string, pair<float, float> > b) {
  return ((a.second.first+a.second.second)
	  > (b.second.first + b.second.second));
}
