#!/bin/bash

set -x

export STARVIWERHOME=/scratch4/NAGAPE/epic/Wei.Huang/src/nv/starviewer
# Force the application to look in the source directory for assets
export STARVIEWER_DIR="/scratch4/NAGAPE/epic/Wei.Huang/src/nv/starviewer"
export STARVIEWER_DATA_PATH="/scratch4/NAGAPE/epic/Wei.Huang/src/nv/starviewer/colormaps"

export LD_PRELOAD=/scratch4/NAGAPE/epic/Wei.Huang/conda-env/weienv/lib/libstdc++.so.6
export XDG_RUNTIME_DIR=/scratch4/NAGAPE/epic/Wei.Huang/tmp/runtime-Wei.Huang
#export QT_DEBUG_PLUGINS=1

# 1. Force Mesa to use CPU rendering
export LIBGL_ALWAYS_SOFTWARE=1
#export GALLIUM_DRIVER=llvmpipe

# 2. Force Qt to use its "Software" Rasterizer instead of OpenGL
# This is the most important toggle for blank windows
#export QT_OPENGL_BACKEND=software
#export QT_QUICK_BACKEND=software

# 3. Tell the XCB plugin to stop looking for GLX integrations
#export QT_XCB_GL_INTEGRATION=none

# 4. Standard X11 hygiene
#export QT_X11_NO_MITSHM=1

export QT_QUICK_BACKEND=software
export LIBGL_ALWAYS_SOFTWARE=1

#/scratch4/NAGAPE/epic/Wei.Huang/src/nv/starviewer/build/main/starviewer -mpi mpi
cd /scratch4/NAGAPE/epic/Wei.Huang/src/nv/starviewer/build/main
./starviewer -mpi mpi
