

#include "MeroHoloModule.hpp"
#include "distance.hpp"
#include "Paths.hpp"
#include "../tools.h"
#include <iostream>
#include <fstream>


using namespace std;

MeroHoloModule::MeroHoloModule() {
}

MeroHoloModule::MeroHoloModule(string& datafile) :
  cntMeros(0), cntHolos(0) {
  initializeDicMap(dicmap);  
  loadMeroHolos(datafile);

  ifstream idss(TYPEROLERFILE);
  string s;
  cerr << "Loading " << TYPEROLERFILE << endl;
  while (getline(idss, s) ) {	
    ulong currentId; 
    stringstream ss;
    ss << s;
    ss >> currentId;
    if (s.find('}') - s.find('{') !=1 && s.find(']')-s.find('[')!=3) {
      processLine(currentId, s.substr(s.find('{')+1), MERO);    
    }
  }
  idss.close();  
  cout << "end load TYPEROLERFILE" << endl;

  idss.open(HOLOFILE);

  cerr << "Loading " << HOLOFILE << endl;
  while (getline(idss, s) ) {	
    ulong currentId; 
    stringstream ss;
    ss << s;
    ss >> currentId;
    if (s.find('}') - s.find('{') !=1 && s.find(']')-s.find('[')!=3) {
      processLine(currentId, s.substr(s.find('{')+1), HOLO);    
    }
  }
  idss.close();  
  cout << "end load HOLOFILE" << endl;
  
}


void MeroHoloModule::loadMeroHolos(string dataInput) {
  ifstream dataIfs(dataInput.c_str());
  string s;
  ulong cntMeros = 0;
  ulong cntHolos = 0;
  while (getline(dataIfs, s) ) {	
    string str = s;
    string synsetId = s.substr(0, s.find(' '));    

    std::string::const_iterator start, end; 
    boost::match_results<std::string::const_iterator> what; 
    boost::match_flag_type flags = boost::match_default; 

    // extract meronyms && holonyms
    start = s.begin(); 
    end = s.end(); 
    while(regex_search(start, end, what, boost::regex("%p ([^ ]*) n"), flags))  { 
      /*      cerr << "-------------------------------" << endl;
      cerr << "INSERT mero 0 : " << synsetId << " -> " << string(what[1].first, what[1].second) << endl;
      */
      if (meronyms.find(synsetId)==meronyms.end()) {
	meronyms[synsetId]=set<string>(); 
      }
      meronyms[synsetId].insert(string(what[1].first, what[1].second)); 
      // update search position: 
      start = what[1].second; 
      // update flags: 
      flags |= boost::match_prev_avail; 
      flags |= boost::match_not_bob; 
    }
    cntMeros+=meronyms[synsetId].size();

    while(regex_search(start, end, what, boost::regex("#p ([^ ]*) n"), flags))  { 
      /*      cerr << "-------------------------------" << endl;
      cerr << "INSERT holo 0 : " << synsetId << " -> " << string(what[1].first, what[1].second) << endl;
      */

      holonyms[synsetId].insert(string(what[1].first, what[1].second)); 
      /*      for (set<string>::iterator it1 = reverseIndex[synsetId].begin(); it1 != reverseIndex[synsetId].end() ; it1 ++) {
	cerr << *it1 << endl;
      }
      cerr << "-->>>--" << endl;
      for (set<string>::iterator it1 = reverseIndex[string(what[1].first, what[1].second)].begin(); it1 != reverseIndex[string(what[1].first, what[1].second)].end() ; it1 ++) {
	cerr << *it1 << endl;
      }
      */


      // update search position: 
      start = what[1].second; 
      // update flags: 
      flags |= boost::match_prev_avail; 
      flags |= boost::match_not_bob; 
    }
    cntHolos+=holonyms[synsetId].size();
  }


  dataIfs.close(); 
  /*
  cerr << "Total Meronyms : " << cntMeros << endl;
  cerr << "Total Holonyms : " << cntHolos << endl;  
  */
}

void MeroHoloModule::processLine(ulong currentId, string s, Mode mode) {  
  stringstream ss;
  ss << s;
  string currentIdCooc;
  uint nbOccs = 0;
  uint sum = 0;
  while (ss.get()!='}') {
    ss.unget();
    ulong currentIdCooc; 
    ss >> currentIdCooc;
    ss.ignore(16, ',');
    ss >> nbOccs;
    ss.ignore(4, ';');
    sum+=nbOccs;
    if (mode==MERO) {
      coocsMero[dicmap[currentId]].insert(make_pair(dicmap[currentIdCooc], nbOccs));      
    } else if (mode==HOLO) {
      coocsHolo[dicmap[currentId]].insert(make_pair(dicmap[currentIdCooc], nbOccs));
    }

  }
  if (mode==MERO) {
    sumMeros[dicmap[currentId]]=sum;
  } else if (mode==HOLO) {
    sumHolos[dicmap[currentId]]=sum;
  }  
  
}









MeroHoloModule::~MeroHoloModule() {
}


void MeroHoloModule::finalize() {
  meronyms.clear();
  holonyms.clear();
  coocsMero.clear();
  coocsHolo.clear();
  sumMeros.clear();
  sumHolos.clear();
}








void MeroHoloModule::process(WORDNET::WordNet& wn, bool verbose){
  for (map<string, WORDNET::WordNetEntry>::iterator itwn = wn.begin(); itwn !=wn.end(); itwn++) {
    for (map<string, set<string> >::iterator itwne = itwn->second.frenchSynset.begin(); itwne !=itwn->second.frenchSynset.end(); itwne++) {	
      //    for (map<string, WORDNET::TgtCandidates>::iterator itlit = itwn->second.frenchCandidates.begin(); itlit !=itwn->second.frenchCandidates.end(); itlit++) {	
      //      for (map<string, int>::iterator itcand = itlit->second.cand.begin() ; itcand != itlit->second.cand.end(); itcand++) {
	reverseIndex[itwn->first].insert(itwne->first); 
	//	cerr << itwn->first << " -> " << itwne->first << endl;
	//      }
    }
  }
  for (map<string, WORDNET::WordNetEntry>::iterator itwn = wn.begin(); itwn !=wn.end(); itwn++) {
    for (map<string, WORDNET::TgtCandidates>::iterator itlit = itwn->second.frenchCandidates.begin(); itlit !=itwn->second.frenchCandidates.end(); itlit++) {	
      if (itlit->second.processed=="" && itlit->second.cand.size()>0) {
	if (itlit->first.find("equaliz") !=string::npos) {
	  cerr << "TRY PROCESS : " << itlit->first << " -> " << itlit->second.cand.size() << endl;
	}
	itwn->second.newdef=trySelecAndReplace(wn, itwn, itlit);
      }
    }      
  }
}



// TODO: OPTIMIZE SCORE : MI ? Z ? or what ?

float MeroHoloModule::computeIsPartOfScore(WORDNET::WordNet& wn, string strA, string strB) {
  if (wn[strB].frenchSynset.size()==0) {
    return 0;
  }
  float sum = 0;
  cerr << "--------------" << endl;
  for (map<string,set<string> >::iterator itlit = wn[strB].frenchSynset.begin(); itlit != wn[strB].frenchSynset.end(); itlit++) {    
    cerr << strA << " is part of " << itlit->first  << " ? " << endl;
    sum += (float)coocsMero[strA][itlit->first]/(float)(sumMeros[strA]*sumHolos[itlit->first]);
    cntMeros++;
  }
  cerr<< "Score : " << sum/(float)wn[strB].frenchSynset.size() << endl;
  cerr << "--------------" << endl;

  return sum/(float)wn[strB].frenchSynset.size(); 
}


float MeroHoloModule::computeIsWholeOfScore(WORDNET::WordNet& wn, string strA, string strB) {
  if (wn[strB].frenchSynset.size()==0) {
    return 0;
  }
  float sum = 0;
  cerr << "--------------" << endl;
  for (map<string,set<string> >::iterator itlit = wn[strB].frenchSynset.begin(); itlit != wn[strB].frenchSynset.end(); itlit++) {
    cerr << strA << " is whole of " << itlit->first  << " ? " << endl;
    sum += (float)coocsHolo[strA][itlit->first]/(float)(sumMeros[itlit->first]*sumHolos[strA]);
    cntHolos++;
  }

  cerr<< "Score : " << sum/(float)wn[strB].frenchSynset.size() << endl;
  cerr << "--------------" << endl;


  return sum/(float)wn[strB].frenchSynset.size(); 
}




string MeroHoloModule::trySelecAndReplace(WORDNET::WordNet& wn, map<string, WORDNET::WordNetEntry>::iterator itwne, map<string, WORDNET::TgtCandidates>::iterator itlit) {
  set<string> elected;

  map<string, float> votes;

  for (map<string,int>::iterator itcand = itlit->second.cand.begin(); itcand!=itlit->second.cand.end(); itcand++) {
    //    cerr << "Processing : " << itcand->first << " - " << itwne->first << endl;    
    for (set<string>::iterator itMero = meronyms[itwne->first].begin(); itMero != meronyms[itwne->first].end(); itMero++) {
      itwne->second.meros.insert(reverseIndex[*itMero].begin(), reverseIndex[*itMero].end());
      cerr << "COMPUTE mero : " << itcand->first << " -> " << *itMero << endl;
      votes[itcand->first]+=computeIsWholeOfScore(wn, itcand->first, *itMero);      
    }
    if (votes[itcand->first]!=0) {
      cerr << "VOTE mero :" << votes[itcand->first] << endl;
    }
    for (set<string>::iterator itHolo = holonyms[itwne->first].begin(); itHolo != holonyms[itwne->first].end(); itHolo++) {      
      itwne->second.holos.insert(reverseIndex[*itHolo].begin(),
				reverseIndex[*itHolo].end());
      cerr << "COMPUTE holo : " << itcand->first << " -> " << *itHolo << endl;
      votes[itcand->first]+=computeIsPartOfScore(wn, itcand->first, *itHolo);
    }
    if (votes[itcand->first]!=0) {
      cerr << "VOTE holo :" << votes[itcand->first] << endl;
    }
  }
  
  float bestVote = 0;
  string elec = "";
  for(map<string, float>::iterator itVotes= votes.begin(); itVotes!=votes.end() ; itVotes++)  {
    if (itVotes->second> bestVote && itVotes->first !="" ) {
      bestVote = itVotes->second;
      elec = itVotes->first;
    }
  }



  if (elec!="") {
    elected.insert(elec);      
    itlit->second.processed="meroholo";
    for (set<string>::iterator itElec = elected.begin(); itElec != elected.end(); itElec++) {
      if (itwne->second.frenchSynset.find(*itElec)==itwne->second.frenchSynset.end()) {
	itwne->second.frenchSynset[*itElec]=set<string>();
      }
      itwne->second.frenchSynset[*itElec].insert( itlit->first);
    }
    return LoaderModule::tgt2TgtDefs[*elected.begin()];
  }
  return "";
}

