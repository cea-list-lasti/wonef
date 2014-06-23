# Compile protobuf headers
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/knn.pb.cc
	COMMAND protoc knn.proto --cpp_out=.
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/repository.pb.cc
	COMMAND protoc repository.proto --cpp_out=.
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# The wonef library simply contains all files
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
	COMMAND mkdir -p ${CMAKE_SOURCE_DIR}/data ${CMAKE_SOURCE_DIR}/logs ${CMAKE_SOURCE_DIR}/profiles)
