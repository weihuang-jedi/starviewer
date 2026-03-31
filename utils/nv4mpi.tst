#!/bin/bash

set -x

# 1. Broaden the Preload to include the GL Dispatcher and GLX
# This covers almost every standard OpenGL/GLX symbol Qt might look for.
MESA_LIB="/scratch4/NAGAPE/epic/Wei.Huang/src/nv/mesa-install/lib64"
CONDA_LIB="/scratch4/NAGAPE/epic/Wei.Huang/conda-env/weienv/lib"

export LD_PRELOAD="$MESA_LIB/libGLdispatch.so.0:$MESA_LIB/libGLX_mesa.so:$MESA_LIB/libGL.so.1:$CONDA_LIB/libstdc++.so.6"

# 2. Clean up and set Library Paths
export LD_LIBRARY_PATH="$MESA_LIB:$CONDA_LIB:$LD_LIBRARY_PATH"

# 3. Force Software Rendering
export LIBGL_ALWAYS_SOFTWARE=1
export GALLIUM_DRIVER=llvmpipe

# 4. Tell Qt to stop trying to use GLX-integration plugins
# This forces it to use the basic X11 surface without the buggy GLX bridge
export QT_XCB_GL_INTEGRATION=none 
export QT_QPA_PLATFORM=xcb

# 5. MobaXterm/X11 Compatibility
export QT_X11_NO_MITSHM=1
export XLIB_SKIP_ARGB_VISUALS=1

# 6. Launch
/scratch4/NAGAPE/epic/Wei.Huang/src/nv/starviewer/build/main/starviewer -mpi mpi

