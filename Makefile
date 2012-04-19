LDFLAGS     = -L/usr/local/lib -lboost_regex -lxerces-c
LD_LIBRARY_PATH = 
CPPFLAGS    = -g 

all:	translateWN buildBilingualDic evalJAWS-WOLF



COMMON_SRC  = TranslatorModule.cpp distance.cpp TypeRoler.cpp Loader.cpp Dumper.cpp SimSynModule.cpp HyperHypoModule.cpp  MeroHoloModule.cpp LastChanceModule.cpp ../src/converter.cpp ../src/tools.cpp WiktHandler.cpp WolfHandler.cpp JawsEvaluatorHandler.cpp Tools.cpp EwnLoader.cpp MeroHoloLikeHyperModule.cpp JawsEvaluatorHandlerBench.cpp BCSBaseHandler.cpp

COMMON_OBJ  = $(COMMON_SRC:%.cpp=%.o)

H_FILES     = TranslatorModule.hpp Tools.hpp JawsEvaluatorHandler.hpp



buildBilingualDic : buildBilingualDic.o  $(COMMON_OBJ) 
	$(CXX) -o buildBilingualDic buildBilingualDic.o $(COMMON_OBJ)  $(LDFLAGS) 

buildBilingualDic.o : buildBilingualDic.cpp $(H_FILES)
	$(CXX) $(CPPFLAGS) $(LDFLAGS)  -c $*.cpp


translateWN: translateWN.o  $(COMMON_OBJ) $(H_FILES)
	$(CXX) -o translateWN translateWN.o $(COMMON_OBJ)  $(LDFLAGS) $(CPPFLAGS)

translateWN.o : translateWN.cpp $(H_FILES)
	$(CXX) $(LDFLAGS) $(CPPFLAGS) -c $*.cpp

evalJAWS-WOLF: evalJAWS-WOLF.o  $(COMMON_OBJ) $(H_FILES)
	$(CXX) -o evalJAWS-WOLF evalJAWS-WOLF.o $(COMMON_OBJ)  $(LDFLAGS)

evalJAWS-WOLF.o : evalJAWS-WOLF.cpp $(H_FILES)
	$(CXX)  $(CPPFLAGS) $(LDFLAGS)  -c $*.cpp



