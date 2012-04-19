#ifndef MEROHOLOLIKEHYPERMODULE_HPP
#define MEROHOLOLIKERHYPERMODULE_HPP


#include "../knn_search.h"
#include "TranslatorModule.hpp"
#include "Loader.hpp"
#include "TypeRoler.hpp"
#include "HyperHypoModule.hpp"
#include "MeroHoloModule.hpp"
#include "boost/regex.hpp"
#include "../WordNet.hpp"


using namespace std;

class MeroHoloLikeHyperModule : public MeroHoloModule {
  
public : 
  MeroHoloLikeHyperModule(string& datafile);
  ~MeroHoloLikeHyperModule();


  void finalize();
  void process(WORDNET::WordNet& wn, TRMode mode, bool verbose=false) ;

 
protected :

  TypeRoler tRoler;


};



#endif
