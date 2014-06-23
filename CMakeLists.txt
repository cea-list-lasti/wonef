project(WoNeF)

cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pg -ggdb -msse4.2 -march=native")
add_definitions(-DDATAPATH_CMAKE="$ENV{DATAPATH}")

# ICU, Boost and protobuf libraries
find_package(ICU COMPONENTS i18n)
include_directories(${ICU_INCLUDE_DIRS})
link_libraries(${ICU_LIBRARIES})

find_package(Boost 1.54.0 COMPONENTS filesystem system regex)
include_directories(${Boost_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARIES})

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIR})
link_libraries(${PROTOBUF_LIBRARIES})
 
# libxml++ has no cmake module, those need to be set in cbuild.sh
include_directories($ENV{LIBXMLPP_INCLUDE_DIR})
link_directories($ENV{LIBXMLPP_LIBRARY})
link_libraries(xml++-1.0)

add_subdirectory(src)
