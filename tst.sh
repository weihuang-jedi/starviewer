#!/bin/bash

set -x

mkdir -p build
cd build
rm -rf *

GLU_LIBRARY=$CONDA_PREFIX/lib/libGL.so.1
export LD_LIBRARY_PATH=$CONDA_PREFIX/lib:$LD_LIBRARY_PATH

installdir=/scratch4/NAGAPE/epic/Wei.Huang/src/nv/starviewer/install
qt5dir=/scratch4/NAGAPE/epic/Wei.Huang/conda-env/weienv/lib

NetCDF_INCLUDE_DIRS=/scratch4/NAGAPE/epic/Wei.Huang/conda-env/weienv/include
NetCDF_LIBRARIES=/scratch4/NAGAPE/epic/Wei.Huang/conda-env/weienv/lib/libnetcdf.so

#cmake ../src -DQt5_DIR=${QT_ROOT} \
# -DGLU_LIBRARY=${GLU_LIBRARY} \
# -DGLU_INCLUDE_DIR=${MESA_ROOT}/include
 cmake ../src \
  -DCMAKE_PREFIX_PATH=${installdir} \
  -DQt5_DIR=${qt5dir} \
  -DGLU_LIBRARY=${GLU_LIBRARY} \
  -DGLU_INCLUDE_DIR=$CONDA_PREFIX/include
 make -j 4

