#!/bin/bash

set -x

# 1. Clean the failed build
rm -rf CMakeCache.txt CMakeFiles/

# 2. Run CMake with the "Kitchen Sink" Linker Flags
cmake ../src \
  -DCMAKE_PREFIX_PATH="/scratch4/NAGAPE/epic/Wei.Huang/src/nv/mesa-install;/scratch4/NAGAPE/epic/Wei.Huang/conda-env/weienv" \
  -DQt5_DIR=/scratch4/NAGAPE/epic/Wei.Huang/conda-env/weienv/lib/cmake/Qt5 \
  -DGLU_LIBRARY=/scratch4/NAGAPE/epic/Wei.Huang/conda-env/weienv/lib/libGLU.so.1 \
  -DGLU_INCLUDE_DIR=/scratch4/NAGAPE/epic/Wei.Huang/src/nv/mesa-install/include \
  -DCMAKE_CXX_FLAGS="-I/scratch4/NAGAPE/epic/Wei.Huang/src/nv/mesa-install/include -I/scratch4/NAGAPE/epic/Wei.Huang/conda-env/weienv/include" \
  -DCMAKE_EXE_LINKER_FLAGS="-L/scratch4/NAGAPE/epic/Wei.Huang/src/nv/mesa-install/lib64 -L/scratch4/NAGAPE/epic/Wei.Huang/conda-env/weienv/lib -L/usr/lib64 -lGL -Wl,--copy-dt-needed-entries -Wl,--allow-shlib-undefined -L/apps/spack-2024-12/linux-rocky9-x86_64/gcc-11.4.1/netcdf-c-4.9.2-dzmdg3ly7avioysvapk37klgegbsq3js/lib"

# Try the build again
# make -j 8

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

