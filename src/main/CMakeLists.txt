file(GLOB_RECURSE SOURCES *.cpp *.h *.ui *.qrc)

set(OpenGL_GL_PREFERENCE GLVND)

# hdfeos fixes: cuErrorOccurred
#find_library(HDFEOS_LIB NAMES hdfeos PATHS /apps/spack-managed/gcc-11.3.1/*/lib)

# Gspread (optional but often needed for NCL symbols)
#find_library(GSPREAD_LIB NAMES gspread PATHS /apps/spack-managed/gcc-11.3.1/*/lib)

# Check if we found them (optional but helpful for debugging)
#if(NOT UDUNITS_LIB)
#    message(WARNING "UDUNITS not found. cdRel2Comp errors may persist.")
#endif()

# 3. Find NCL Libraries
#find_library(NCL_LIB NAMES ncl PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)

# 4. Standard dependencies often required by static NCL
#find_package(ZLIB REQUIRED)
#find_library(SZIP_LIB NAMES sz szip)
#find_library(CURL_LIB NAMES curl)
#find_library(HLU_LIB NAMES hlu PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)
#find_library(GKS_LIB NAMES ncarg_gks PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)
#find_library(NFP_LIB NAMES nfp PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)
#find_library(NCARG_LIB NAMES ncarg PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)
#find_library(NGMATH_LIB NAMES ngmath PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)
#find_library(COMMON_LIB NAMES ncarg_c PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)
#find_library(SPHERE_LIB NAMES sphere3.1_dp PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)
#find_library(NFPFORT_LIB NAMES nfpfort PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)
#find_library(FFTPACK_LIB NAMES fftpack5_dp PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)
#find_library(LAPACK_LIB NAMES lapack_ncl PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)
#find_library(BLAS_LIB NAMES blas_ncl PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)

# Look for the NCL I/O and Interpreter library
#find_library(NI_LIB NAMES nio PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)

#find_package(Freetype REQUIRED)

# Find Cairo (usually available on HPC via pkg-config or system paths)
#find_package(PkgConfig REQUIRED)
#pkg_check_modules(CAIRO REQUIRED cairo)

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
#find_path(GLU_INCLUDE_DIR NAMES GL/glu.h
#    PATHS /apps/spack-managed/gcc-11.3.1/mesa-glu-9.0.2-jt4lynuhi3xlsnsm6v77irx4g2dxujui/include
#    PATH_SUFFIXES GL
#)

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
  # ${CMAKE_SOURCE_DIR}/coastline2
    ${CMAKE_SOURCE_DIR}/topography
  # ${CMAKE_SOURCE_DIR}/nclapp
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
  # ${CMAKE_SOURCE_DIR}/wrf
    ${CMAKE_SOURCE_DIR}/ufs
  # ${CMAKE_SOURCE_DIR}/pop
  # ${CMAKE_SOURCE_DIR}/camse
  # ${CMAKE_SOURCE_DIR}/hdf
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
  # wrf
    ufs
  # mpas
  # camse
  # pop
  # hdf
  # nclapp
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
    -Wl,--end-group

    # 4. System Graphics and Fortran Runtimes
  # ${FREETYPE_LIBRARIES}
    X11 Xext Xrender
    gfortran        # VITAL for the Fortran symbols in NCL and shared
    OpenMP::OpenMP_CXX

    # 5. Data Formats and Math
    ${NetCDF_LIBRARIES}
    ${HDF5_HL_LIB}
    ${HDF5_LIB}
    ${ZLIB_LIBRARIES}
    gfortran
    yaml-cpp
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
