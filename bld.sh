#!/bin/bash

set -x

nproc=8

mkdir -p build
cd build
rm -rf *

NetCDF_INCLUDE_DIRS=/scratch4/NAGAPE/epic/Wei.Huang/conda-env/weienv/include
NetCDF_LIBRARIES=/scratch4/NAGAPE/epic/Wei.Huang/conda-env/weienv/lib/libnetcdf.so

cmake ../src \
  -DCMAKE_PREFIX_PATH=$CONDA_PREFIX \
  -DCMAKE_FIND_ROOT_PATH=$CONDA_PREFIX \
  -DOPENGL_gl_LIBRARY="$CONDA_PREFIX/lib/libGL.so" \
  -DOPENGL_glu_LIBRARY="$CONDA_PREFIX/lib/libGLU.so" \
  -DCMAKE_EXE_LINKER_FLAGS="-L$CONDA_PREFIX/lib -Wl,-rpath,$CONDA_PREFIX/lib -lnetcdf -lhdf5 -lGL -lGLU -lX11 -lXext"

# Build again
 make -j${nproc}

#make LDLIBS="-lGLU -lGL" -j $(nproc)

