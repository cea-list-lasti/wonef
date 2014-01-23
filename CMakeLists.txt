project(WoNeF)

cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pg -ggdb")

# All used libraries. xml++ has no cmake modules, so we include it manually
find_package(ICU COMPONENTS i18n)
include_directories(${ICU_INCLUDE_DIRS})
link_libraries(${ICU_LIBRARIES})

find_package(Boost 1.54.0 COMPONENTS filesystem system regex)
include_directories(${Boost_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARIES})

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIR})
link_libraries(${PROTOBUF_LIBRARIES})

include_directories($ENV{LIBXMLPP_INCLUDE_DIR})
link_directories($ENV{LIBXMLPP_LIBRARY})
link_libraries(xml++-1.0)

# The wonef library simply contains all object codes
add_library(wonef
    ExtractorModule.cpp SimSynModule.cpp HyperHypoModule.cpp MeroHoloModule.cpp
    LastChanceModule.cpp MeroHoloLikeHyperModule.cpp TranslatorModule.cpp
    BestTranslations.cpp

    WordNet.cpp Paths.hpp
    knn.pb.cc repository.pb.cc
    Options.cpp

    Dictionaries.cpp TypeRoler.cpp Tools.cpp tools.cpp levenshtein.cpp

    DEBVisDicDumper.cpp Dumper.cpp

    Loader.cpp GoldHandler.cpp JawsHandler.cpp	
	WolfHandler.cpp WolfOneHandler.cpp EwnLoader.cpp BCSBaseHandler.cpp
)

link_libraries(wonef)

add_executable(translateNouns translateNouns.cpp)
add_executable(translateVerbs translateVerbs.cpp)
add_executable(translateAdjs translateAdjs.cpp)
add_executable(translateAdvs translateAdvs.cpp)
add_executable(evalJAWS-WOLF evalJAWS-WOLF.cpp)

add_custom_command(TARGET translateNouns POST_BUILD
	COMMAND mkdir -p ${CMAKE_SOURCE_DIR}/data ${CMAKE_SOURCE_DIR}/logs)
