#!/bin/bash

set -x

# Clean the failed cache
rm -rf CMakeCache.txt CMakeFiles/

# Run CMake with high-priority include paths
cmake ../src \
  -DCMAKE_PREFIX_PATH="/scratch4/NAGAPE/epic/Wei.Huang/src/nv/mesa-install;/scratch4/NAGAPE/epic/Wei.Huang/conda-env/weienv" \
  -DQt5_DIR=/scratch4/NAGAPE/epic/Wei.Huang/conda-env/weienv/lib/cmake/Qt5 \
  -DGLU_LIBRARY=/usr/lib64/libGLU.so.1 \
  -DGLU_INCLUDE_DIR=/scratch4/NAGAPE/epic/Wei.Huang/src/nv/mesa-install/include \
  -DCMAKE_CXX_FLAGS="-I/scratch4/NAGAPE/epic/Wei.Huang/src/nv/mesa-install/include"

# Try to build again
make -j 8

exit 0
# 1. Create the target directory in your custom Mesa install
#mkdir -p /scratch4/NAGAPE/epic/Wei.Huang/src/nv/mesa-install/include/GL

# 2. Find and copy the GL headers from your Mesa SOURCE folder
# (Adjust the source path if your mesa-26.0.3 folder is elsewhere)
#cp /scratch4/NAGAPE/epic/Wei.Huang/src/nv/mesa-26.0.3/include/GL/*.h \
#   /scratch4/NAGAPE/epic/Wei.Huang/src/nv/mesa-install/include/GL/

# 3. Create the symlink in Conda so Qt is happy
#mkdir -p /scratch4/NAGAPE/epic/Wei.Huang/conda-env/weienv/include/GL
#ln -sf /scratch4/NAGAPE/epic/Wei.Huang/src/nv/mesa-install/include/GL/gl.h \
#       /scratch4/NAGAPE/epic/Wei.Huang/conda-env/weienv/include/GL/gl.h
#

