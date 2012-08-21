LDFLAGS     = -L/usr/local/lib -L/home/qp230782/externals/xerces-c-3.1.1/lib -L/home/qp230782/externals/boost_1_49_0/stage/lib -L/home/qp230782/externals/protobuf/lib -I/home/qp230782/externals/protobuf/include -lboost_regex -lxerces-c -lprotobuf -lpthread -pg
CPPFLAGS    = -Wall -Wextra -O3 -I/home/qp230782/externals/boost_1_49_0/ -I/home/qp230782/externals/xerces-c-3.1.1/include -I/home/qp230782/externals/protobuf/include -pg

all:	translateWN translateVerbs translateAdjs buildBilingualDic evalJAWS-WOLF

COMMON_SRC  = TranslatorModule.cpp distance.cpp TypeRoler.cpp Loader.cpp Dumper.cpp SimSynModule.cpp HyperHypoModule.cpp MeroHoloModule.cpp LastChanceModule.cpp ../src/converter.cpp ../src/tools.cpp WiktHandler.cpp WolfHandler.cpp JawsEvaluatorHandler.cpp Tools.cpp EwnLoader.cpp MeroHoloLikeHyperModule.cpp JawsEvaluatorHandlerBench.cpp BCSBaseHandler.cpp BestTranslations.cpp repository.pb.cc GoldHandler.cpp JawsHandler.cpp

COMMON_OBJ  = $(COMMON_SRC:%.cpp=%.o)

H_FILES     = $(wildcard *.hpp) repository.pb.h
PROTO_FILES = $(wildcard *.proto)

data2:
	mkdir data2
logs:
	mkdir logs

repository.pb.h: $(PROTO_FILES)
	protoc repository.proto --cpp_out=.

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

translateAdjs: translateAdjs.o  $(COMMON_OBJ) $(H_FILES)
	$(CXX) -o translateAdjs translateAdjs.o $(COMMON_OBJ)  $(LDFLAGS) $(CPPFLAGS)

translateAdjs.o : translateAdjs.cpp $(H_FILES)
	$(CXX) $(CPPFLAGS) -c $*.cpp

evalJAWS-WOLF: evalJAWS-WOLF.o  $(COMMON_OBJ) $(H_FILES)
	$(CXX) -o evalJAWS-WOLF evalJAWS-WOLF.o $(COMMON_OBJ)  $(LDFLAGS)

evalJAWS-WOLF.o : evalJAWS-WOLF.cpp $(H_FILES)
	$(CXX) $(CPPFLAGS) -c $*.cpp
