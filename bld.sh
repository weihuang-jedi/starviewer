#!/bin/bash

 set -x

 module load qt/5.15.14
 module load ncl/6.6.2
#module load netcdf-c/4.9.0

 mkdir -p build

 cd build
 rm -rf *
 cmake ../src -DQt5_DIR=/apps/contrib/spack-stack-1.1/gcc-11.3.1/qt-5.15.14-mfeuvcidmyqoi2m5i2tfrk6yd7xtk6pt \
	 -DGLU_LIBRARY=/usr/lib64/libGLU.so.1 \
	 -DGLU_INCLUDE_DIR=/apps/spack-managed/gcc-11.3.1/mesa-glu-9.0.2-jt4lynuhi3xlsnsm6v77irx4g2dxujui/include
 make
