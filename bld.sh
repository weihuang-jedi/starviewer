#!/bin/bash

set -x

nproc=8

mkdir -p build
cd build
rm -rf *

# Set these so CMake's find_package can work automatically
export CXX=/usr/bin/g++
export CC=/usr/bin/gcc
# Add the Fortran compiler export
export FC=/usr/bin/gfortran

cmake ../src \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH=/usr/lib/x86_64-linux-gnu/cmake/Qt5 \
  "-DCMAKE_EXE_LINKER_FLAGS=-Wl,-rpath,/usr/lib/x86_64-linux-gnu -lGL -lGLU -lX11 -lXrender" \
  -DCMAKE_CXX_COMPILER_WORKS=1 \
  -DCMAKE_C_COMPILER_WORKS=1 \
  -DCMAKE_Fortran_COMPILER_WORKS=1 \
  -DNetCDF_INCLUDE_DIR=/usr/include \
  -DNetCDF_LIBRARY=/usr/lib/x86_64-linux-gnu/libnetcdf.so \
  -DCMAKE_INSTALL_RPATH="/usr/lib/x86_64-linux-gnu"

make -j${nproc}


