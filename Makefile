LDFLAGS     = -L/usr/local/lib  -L/home/pradet/externals/xerces-c/lib -L/home/pradet/externals/icu/lib -L/home/pradet/externals/boost_1_51_0/stage/lib -L/home/pradet/externals/fastsax/libxml++/lib -L/home/pradet/externals/protobuf/lib -I/home/pradet/externals/protobuf/include -licuuc -licudata -licui18n -lboost_system -lboost_regex -lboost_filesystem -liconv -lxerces-c -lprotobuf -lpthread -lxml++-1.0 -pg
CPPFLAGS    = -std=c++11 -Wall -Wextra -I/home/pradet/externals/boost_1_51_0 -I/home/pradet/externals/icu/include -I/home/pradet/externals/xerces-c/include -I/home/pradet/externals/protobuf/include -I/home/pradet/externals/fastsax/libxml++/include/libxml++-1.0 -pg -O3

all:	translateNouns translateVerbs translateAdjs buildBilingualDic evalJAWS-WOLF

COMMON_SRC  = TranslatorModule.cpp distance.cpp TypeRoler.cpp Loader.cpp Dumper.cpp ExtractorModule.cpp SimSynModule.cpp HyperHypoModule.cpp MeroHoloModule.cpp LastChanceModule.cpp ../src/tools.cpp WiktHandler.cpp WolfHandler.cpp Tools.cpp EwnLoader.cpp MeroHoloLikeHyperModule.cpp BCSBaseHandler.cpp BestTranslations.cpp knn.pb.cc repository.pb.cc GoldHandler.cpp JawsHandler.cpp Options.cpp

COMMON_OBJ  = $(COMMON_SRC:%.cpp=%.o)

H_FILES     = $(wildcard *.hpp) repository.pb.h knn.pb.h
PROTO_FILES = $(wildcard *.proto)

data2:
	mkdir data2
logs:
	mkdir logs

repository.pb.h: $(PROTO_FILES)
	protoc repository.proto --cpp_out=.

knn.pb.h: $(PROTO_FILES)
	protoc knn.proto --cpp_out=.

buildBilingualDic : buildBilingualDic.o  $(COMMON_OBJ) 
	$(CXX) -o buildBilingualDic buildBilingualDic.o $(COMMON_OBJ)  $(LDFLAGS) 

buildBilingualDic.o : buildBilingualDic.cpp $(H_FILES)
	$(CXX) $(CPPFLAGS) -c $*.cpp

translateNouns: translateNouns.o  $(COMMON_OBJ) $(H_FILES)
	$(CXX) -o translateNouns translateNouns.o $(COMMON_OBJ)  $(LDFLAGS) $(CPPFLAGS)

translateNouns.o : translateNouns.cpp $(H_FILES)
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
