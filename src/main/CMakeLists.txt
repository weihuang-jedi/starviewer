file(GLOB_RECURSE SOURCES *.cpp *.h *.ui *.qrc)

set(OpenGL_GL_PREFERENCE GLVND)
set(NetCDF_INCLUDE_DIRS $ENV{NETCDF_C_ROOT}/include)
set(NetCDF_LIBRARIES $ENV{NETCDF_C_ROOT}/lib/libnetcdf.so)
find_path(NetCDF_INCLUDE_DIRS NAMES netcdf.h)
find_library(NetCDF_LIBRARIES NAMES netcdf)

# --- NCL and Dependencies Setup ---

# 1. Try to find NCARG_ROOT from environment
if(NOT DEFINED ENV{NCARG_ROOT})
    message(FATAL_ERROR "NCARG_ROOT environment variable not set. Please load the ncl module.")
endif()
set(NCL_LIB_DIR $ENV{NCARG_ROOT}/lib)
set(NCL_INC_DIR $ENV{NCARG_ROOT}/include)

# 2. Find HDF5 (Required by NCL)
# We use the path identified in your previous error
set(HDF5_ROOT "/apps/spack-managed/gcc-11.3.1/hdf5-1.12.2-w7j5cujjvt45evmlqi6yyff4q4qqjees")
find_library(HDF5_LIB NAMES hdf5 PATHS ${HDF5_ROOT}/lib NO_DEFAULT_PATH)
find_library(HDF5_HL_LIB NAMES hdf5_hl PATHS ${HDF5_ROOT}/lib NO_DEFAULT_PATH)

# Replace these with the actual paths you found from the 'spack find' command
#set(UDUNITS_LIB "/apps/spack-managed/gcc-11.3.1/udunits-2.2.28-yejcsju74d43ulira4kz7flaghyd5cmf/lib/libudunits2.a")
#set(HDFEOS_LIB  "/path/from/spack/lib/libhdfeos.a")

# --- Find NCL-specific helper libraries ---
# udunits2 fixes: cdRel2Comp, calendar_type
find_library(UDUNITS_LIB NAMES udunits2 PATHS /apps/spack-managed/gcc-11.3.1/*/lib)

# hdfeos fixes: cuErrorOccurred
#find_library(HDFEOS_LIB NAMES hdfeos PATHS /apps/spack-managed/gcc-11.3.1/*/lib)

# Gspread (optional but often needed for NCL symbols)
#find_library(GSPREAD_LIB NAMES gspread PATHS /apps/spack-managed/gcc-11.3.1/*/lib)

# Check if we found them (optional but helpful for debugging)
if(NOT UDUNITS_LIB)
    message(WARNING "UDUNITS not found. cdRel2Comp errors may persist.")
endif()

# 3. Find NCL Libraries
find_library(NCL_LIB NAMES ncl PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)

# 4. Standard dependencies often required by static NCL
find_package(ZLIB REQUIRED)
find_library(SZIP_LIB NAMES sz szip)
find_library(CURL_LIB NAMES curl)
find_library(HLU_LIB NAMES hlu PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)
find_library(GKS_LIB NAMES ncarg_gks PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)
find_library(NFP_LIB NAMES nfp PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)
find_library(NCARG_LIB NAMES ncarg PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)
find_library(NGMATH_LIB NAMES ngmath PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)
find_library(COMMON_LIB NAMES ncarg_c PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)
find_library(SPHERE_LIB NAMES sphere3.1_dp PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)
find_library(NFPFORT_LIB NAMES nfpfort PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)
find_library(FFTPACK_LIB NAMES fftpack5_dp PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)
find_library(LAPACK_LIB NAMES lapack_ncl PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)
find_library(BLAS_LIB NAMES blas_ncl PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)

# Look for the NCL I/O and Interpreter library
find_library(NI_LIB NAMES nio PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)

find_package(Freetype REQUIRED)

# Find Cairo (usually available on HPC via pkg-config or system paths)
find_package(PkgConfig REQUIRED)
pkg_check_modules(CAIRO REQUIRED cairo)

# Find OpenMP
find_package(OpenMP REQUIRED)

# 1. Find Core Qt5
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
# 2. Find the specific OpenGL component explicitly
find_package(Qt5OpenGL REQUIRED)

# Specifically look for the GLU library file
find_library(GLU_LIBRARY NAMES GLU glu)

# Manually search for the directory containing GL/glu.h
find_path(GLU_INCLUDE_DIR NAMES GL/glu.h
    PATHS /apps/spack-managed/gcc-11.3.1/mesa-glu-9.0.2-jt4lynuhi3xlsnsm6v77irx4g2dxujui/include
    PATH_SUFFIXES GL
)

# Create a library for this module
# add_library(starviewer STATIC ${SOURCES})
add_executable(starviewer main.cpp mainwindow.cpp)

# Allow other modules to find these headers
target_include_directories(starviewer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${GLU_INCLUDE_DIR}
    ${GLU_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/geometry
    ${CMAKE_SOURCE_DIR}/shared
    ${CMAKE_SOURCE_DIR}/coastline
    ${CMAKE_SOURCE_DIR}/coastline2
    ${CMAKE_SOURCE_DIR}/topography
    ${CMAKE_SOURCE_DIR}/nclapp
    ${CMAKE_SOURCE_DIR}/ncreader
    ${CMAKE_SOURCE_DIR}/stateboundary
    ${CMAKE_SOURCE_DIR}/contour
    ${CMAKE_SOURCE_DIR}/control
    ${CMAKE_SOURCE_DIR}/view
    ${CMAKE_SOURCE_DIR}/windvector
    ${CMAKE_SOURCE_DIR}/marchingCube
    ${CMAKE_SOURCE_DIR}/translator
    ${CMAKE_SOURCE_DIR}/trajectory
    ${CMAKE_SOURCE_DIR}/widget
    ${CMAKE_SOURCE_DIR}/lic
    ${CMAKE_SOURCE_DIR}/wrf
    ${CMAKE_SOURCE_DIR}/ufs
    ${CMAKE_SOURCE_DIR}/pop
    ${CMAKE_SOURCE_DIR}/camse
    ${CMAKE_SOURCE_DIR}/hdf
    ${CMAKE_SOURCE_DIR}/test
    ${CMAKE_SOURCE_DIR}/mpidemo
    ${NetCDF_INCLUDE_DIRS}
)

# message("HLU_LIB: ${HLU_LIB}")
# message("GKS_LIB: ${GKS_LIB}")
# message("NCARG_LIB: ${NCARG_LIB}")
# message("NGMATH_LIB: ${NGMATH_LIB}")
# message("COMMON_LIB: ${COMMON_LIB}")

target_link_libraries(starviewer PRIVATE
    # 1. High-level UI Modules (Must come before main/executable)
    control
    translator      # Fixes TestTranslator and MPITranslator
    widget          # Fixes UI components
    mpidemo         # Fixes mpidemo symbols

    # 2. Logic and Data Modules
    test
    wrf
    ufs
    mpas
    camse
    pop
    hdf
    nclapp
    ncreader
    view
    control
    contour
    shared
    geometry
    trajectory
    windvector
    topography
    coastline
    coastline2
    marchingCube
    leveler
    lic
    picking
    spreadsheet
    stateboundary
    control
    view
    leveler

    # 3. THE NCL CORE GROUP (Critical for get_dimensions and ESprintf)
    -Wl,--start-group
    ${NCL_LIB}      # libncl.a
    ${NI_LIB}       # libnio.a
    ${HLU_LIB}      # libhlu.a
  # ${NCARG_LIB}    # libncarg.a
  # ${COMMON_LIB}   # libncarg_c.a
    ${UDUNITS_LIB}  # libudunits2.a
   #${HDFEOS_LIB}   # libhdfeos.a
   #${GSPREAD_LIB}
    -Wl,--end-group

    # 4. System Graphics and Fortran Runtimes
    ${GKS_LIB}
    ${NFPFORT_LIB}
    ${SPHERE_LIB}
    ${NGMATH_LIB}
    ${CAIRO_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${NFP_LIB}
  # ${COMMON_LIB}   # libncarg_c.a
    X11 Xext Xrender
    gfortran        # VITAL for the Fortran symbols in NCL and shared
    OpenMP::OpenMP_CXX

    # 5. Data Formats and Math
    ${NetCDF_LIBRARIES}
    ${HDF5_HL_LIB}
    ${HDF5_LIB}
    ${ZLIB_LIBRARIES}
    ${NCL_LIB}      # libncl.a
    ${NCARG_LIB}    # libncarg.a
    ${COMMON_LIB}   # libncarg_c.a
    ${FFTPACK_LIB}    # libfftpack5_dp.a
    ${LAPACK_LIB}    # liblapack_ncl.a
    ${BLAS_LIB}    # libblas_ncl.a
    gfortran
    curl
    m
    dl

    # 6. Qt Framework
    Qt5::Widgets
    Qt5::OpenGL
)

# 7. Add the Multiple Definition Flag
set_target_properties(starviewer PROPERTIES
    LINK_FLAGS "-Wl,--allow-multiple-definition"
)
