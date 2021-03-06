#ifndef MEROHOLOLIKEHYPERMODULE_HPP
#define MEROHOLOLIKERHYPERMODULE_HPP


#include "TranslatorModule.hpp"
#include "Loader.hpp"
#include "TypeRoler.hpp"
#include "HyperHypoModule.hpp"
#include "MeroHoloModule.hpp"
#include <boost/regex.hpp>
#include "WordNet.hpp"



class MeroHoloLikeHyperModule : public MeroHoloModule {

public:
  MeroHoloLikeHyperModule(const std::string& datafile, int idModuleConf, int nIteration);
  virtual ~MeroHoloLikeHyperModule();


  void finalize();
  virtual void process(WORDNET::WordNet& wn, TRMode mode, bool verbose=false) ;


protected:

  TypeRoler tRoler;


};



#endif
