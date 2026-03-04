#!/bin/bash

 set -x

#module load qt/5.15.14
#module load ncl/6.6.2

 mkdir build

 cd build

 cmake .. -DQt5_DIR=/apps/contrib/spack-stack-1.1/gcc-11.3.1/qt-5.15.14-mfeuvcidmyqoi2m5i2tfrk6yd7xtk6pt
