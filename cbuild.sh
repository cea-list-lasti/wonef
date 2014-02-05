mkdir -p build
cd build

# libxml++
export LIBXMLPP_INCLUDE_DIR=$HOME/Software/libxml++/include/libxml++-1.0
export LIBXMLPP_LIBRARY=$HOME/Software/libxml++/lib

cmake ..

make -j8
