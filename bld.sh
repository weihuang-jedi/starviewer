#!/bin/bash

set -x

mkdir -p build
cd build
rm -rf *

# First detect w/ hostname
case $(hostname -f) in
    ufe0[1-9] | ufe1[0-6] | uecflow01)
        MODULE_NAME="rdhpcs-conda"
        if lsmod | awk '{print $1}' | grep -qx "$MODULE_NAME"; then
            echo "Module '$MODULE_NAME' is loaded."
        else
          # module load rdhpcs-conda
            # conda create -p weienv python=3.13
          # conda activate /scratch4/NAGAPE/epic/Wei.Huang/conda-env/weienv
            # conda deactivate
	    # pip install PyQt5
	    # pip install mesa
	    # pip install PyOpenGL PyOpenGL_accelerate
          # module purge
            module use moduledir
            module load ursa.gnu 
	fi
      # GLU_LIBRARY=/lib64/libGL.so.1
	GLU_LIBRARY=$CONDA_PREFIX/lib/libGL.so.1
      # export PKG_CONFIG_PATH=$CONDA_PREFIX/lib/pkgconfig:$CONDA_PREFIX/share/pkgconfig:$PKG_CONFIG_PATH
      # Define your install path
        export MESA_ROOT=/scratch4/NAGAPE/epic/Wei.Huang/src/nv/mesa-install

        # 1. Point to the new libraries (OpenGL, EGL, GBM)
        export LD_LIBRARY_PATH=$MESA_ROOT/lib64:$CONDA_PREFIX/lib:$LD_LIBRARY_PATH

        # 2. Tell Mesa where the hardware-specific "drivers" (.so files) are
        export LIBGL_DRIVERS_PATH=$MESA_ROOT/lib64/dri

        # This tells GLVND to use your specific Mesa library
        export __GLX_VENDOR_LIBRARY_NAME=mesa

        # 3. Tell the system where the EGL/Vulkan JSON manifests are
        export EGL_DRIVERS_PATH=$MESA_ROOT/lib64/dri
        export VK_ICD_FILENAMES=$MESA_ROOT/share/vulkan/icd.d/nouveau_icd.x86_64.json
        installdir=/scratch4/NAGAPE/epic/Wei.Huang/src/nv/starviewer/install
      # qt5dir=/scratch4/NAGAPE/epic/Wei.Huang/conda-env/weienv/lib
        qt5dir=/scratch4/NAGAPE/epic/Wei.Huang/conda-env/weienv/lib

        # 4. Optional: Force a specific renderer for testing
        # export GALLIUM_DRIVER=nouveau  # Force NVIDIA
        # export GALLIUM_DRIVER=llvmpipe # Force CPU

        ;;
    [Hh]ercules-login-[1-4].[Hh][Pp][Cc].[Mm]s[Ss]tate.[Ee]du)
        MODULE_NAME="hercules.gnu"
        if lsmod | awk '{print $1}' | grep -qx "$MODULE_NAME"; then
            echo "Module '$MODULE_NAME' is loaded."
        else
            module purge
            module use moduledir
            module load hercules.gnu 
        fi
	GLU_LIBRARY=/usr/lib64/libGLU.so.1
        ;;
    *) MACHINE_ID=UNKNOWN ;;                        # Unknown platform
esac
 echo "QT_ROOT: ${QT_ROOT}"
#cmake ../src -DQt5_DIR=${QT_ROOT} \
# -DGLU_LIBRARY=${GLU_LIBRARY} \
# -DGLU_INCLUDE_DIR=${MESA_ROOT}/include
 cmake ../src \
  -DCMAKE_PREFIX_PATH=${installdir} \
  -DQt5_DIR=${qt5dir} \
  -DGLU_LIBRARY=${GLU_LIBRARY} \
  -DGLU_INCLUDE_DIR=/scratch4/NAGAPE/epic/Wei.Huang/src/nv/mesa-install/include
 make -j 4

