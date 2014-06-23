#ifndef HYPERHYPOMODULE_HPP
#define HYPERHYPOMODULE_HPP

#include "TranslatorModule.hpp"
#include "TypeRoler.hpp"



class HyperHypoModule : TranslatorModule {
public:
  HyperHypoModule(std::string dataInput, std::string typeroler, TRMode _mode, std::string _pos, int idModuleConf, int nIteration, bool verbose=false);
  virtual ~HyperHypoModule();

  virtual void process(WORDNET::WordNet& wn) ;
  static std::string getHead (std::string locution);

private:
  TRMode mode;
  TypeRoler tRoler;
  std::map<std::string, std::set<std::string> >hypernyms;
  std::map<std::string, std::set<std::string> >hyponyms;

  std::string pos;
  std::string suffix;

  void loadHyperHypos(std::string dataInput);

};



#endif
