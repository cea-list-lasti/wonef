LDFLAGS     = -L/usr/local/lib -L/home/qp230782/externals/xerces-c-3.1.1/lib -L/home/qp230782/externals/boost_1_49_0/stage/lib -lboost_regex -lxerces-c -pg
CPPFLAGS    = -Wall -Wextra -O3 -I/home/qp230782/externals/boost_1_49_0/ -I/home/qp230782/externals/xerces-c-3.1.1/include -pg

all:	translateWN translateVerbs buildBilingualDic evalJAWS-WOLF evalVerbsJAWS-WOLF

COMMON_SRC  = TranslatorModule.cpp distance.cpp TypeRoler.cpp Loader.cpp Dumper.cpp SimSynModule.cpp HyperHypoModule.cpp MeroHoloModule.cpp LastChanceModule.cpp ../src/converter.cpp ../src/tools.cpp WiktHandler.cpp WolfHandler.cpp JawsEvaluatorHandler.cpp Tools.cpp EwnLoader.cpp MeroHoloLikeHyperModule.cpp JawsEvaluatorHandlerBench.cpp BCSBaseHandler.cpp BestTranslations.cpp

COMMON_OBJ  = $(COMMON_SRC:%.cpp=%.o)

H_FILES     = $(wildcard *.hpp)



buildBilingualDic : buildBilingualDic.o  $(COMMON_OBJ) 
	$(CXX) -o buildBilingualDic buildBilingualDic.o $(COMMON_OBJ)  $(LDFLAGS) 

buildBilingualDic.o : buildBilingualDic.cpp $(H_FILES)
	$(CXX) $(CPPFLAGS) -c $*.cpp

translateWN: translateWN.o  $(COMMON_OBJ) $(H_FILES)
	$(CXX) -o translateWN translateWN.o $(COMMON_OBJ)  $(LDFLAGS) $(CPPFLAGS)

translateWN.o : translateWN.cpp $(H_FILES)
	$(CXX) $(CPPFLAGS) -c $*.cpp

translateVerbs: translateVerbs.o  $(COMMON_OBJ) $(H_FILES)
	$(CXX) -o translateVerbs translateVerbs.o $(COMMON_OBJ)  $(LDFLAGS) $(CPPFLAGS)

translateVerbs.o : translateVerbs.cpp $(H_FILES)
	$(CXX) $(CPPFLAGS) -c $*.cpp

evalJAWS-WOLF: evalJAWS-WOLF.o  $(COMMON_OBJ) $(H_FILES)
	$(CXX) -o evalJAWS-WOLF evalJAWS-WOLF.o $(COMMON_OBJ)  $(LDFLAGS)

evalJAWS-WOLF.o : evalJAWS-WOLF.cpp $(H_FILES)
	$(CXX)  $(CPPFLAGS) -c $*.cpp

evalVerbsJAWS-WOLF: evalVerbsJAWS-WOLF.o  $(COMMON_OBJ) $(H_FILES)
	$(CXX) -o evalVerbsJAWS-WOLF evalVerbsJAWS-WOLF.o $(COMMON_OBJ)  $(LDFLAGS)

evalVerbsJAWS-WOLF.o : evalVerbsJAWS-WOLF.cpp $(H_FILES)
	$(CXX)  $(CPPFLAGS) -c $*.cpp

