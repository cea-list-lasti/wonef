mkdir -p build
cd build

export ICU_ROOT=/opt/icu_52.1/
export BOOST_ROOT=/opt/boost-1.54.0/
# Protobuf
export CMAKE_INCLUDE_PATH=~/apps/wonef_install/include CMAKE_LIBRARY_PATH=~/apps/wonef_install/lib/
# libxml++
export LIBXMLPP_LIBRARY=$HOME/apps/wonef_install/lib/ LIBXMLPP_INCLUDE_DIR=$HOME/apps/wonef_install/include/libxml++-1.0/

export DATAPATH='/scratch_global/DATASETS/WoNeF'

cmake ..

make -j8
