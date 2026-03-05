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

# 3. Find NCL Libraries
find_library(NCL_LIB NAMES ncl PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)

# 4. Standard dependencies often required by static NCL
find_package(ZLIB REQUIRED)
find_library(SZIP_LIB NAMES sz szip)
find_library(CURL_LIB NAMES curl)
find_library(HLU_LIB NAMES hlu PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)
find_library(NCARG_LIB NAMES ncarg PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)
find_library(GKS_LIB NAMES ncarg_gks PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)
find_library(NGMATH_LIB NAMES ngmath PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)
find_library(COMMON_LIB NAMES ncarg_c PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)

# Look for the internal NCL interpreter library
# Change this line:
find_library(NI_LIB NAMES ni PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)

# To this (Check if it exists before using it):
find_library(NI_LIB NAMES ni PATHS ${NCL_LIB_DIR} NO_DEFAULT_PATH)
if(NOT NI_LIB)
    set(NI_LIB "") # Set to empty string so it doesn't trigger "NOTFOUND"
endif()

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
    ${CMAKE_SOURCE_DIR}/pop
    ${CMAKE_SOURCE_DIR}/camse
    ${CMAKE_SOURCE_DIR}/hdf
    ${CMAKE_SOURCE_DIR}/test
    ${CMAKE_SOURCE_DIR}/mpidemo
    ${NetCDF_INCLUDE_DIRS}
)

#   mpas
target_link_libraries(starviewer PRIVATE
    # 1. UI and High-Level View Logic
    view
    control

    # 2. Logic Engines (The things View/Control call)
    contour        # Fixes Contour:: references
    marchingCube   # Fixes Leveler:: references (since Leveler is in marchingCube)

    # 3. Base Internal Logic
    shared
    geometry
    nclapp
    translator
    wrf
    camse
    pop
    hdf

    # 4. NCL Core
    ${NCL_LIB}
    ${NI_LIB}       # Will be empty if not found, which is fine

    # 5. NCL Support & Graphics (Order: HLU -> NCARG -> GKS)
    ${HLU_LIB}
    ${NCARG_LIB}
    ${GKS_LIB}
    ${NGMATH_LIB}
    ${COMMON_LIB}

    # 6. System Graphics Engines (Order: Cairo -> FreeType -> X11)
    ${CAIRO_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    X11
    Xext
    Xrender

    # 7. Language & Parallelism
    gfortran
    OpenMP::OpenMP_CXX

    # 8. System Formats & Math
    ${NetCDF_LIBRARIES}
    ${HDF5_HL_LIB}
    ${HDF5_LIB}
    ${ZLIB_LIBRARIES}
    ${SZIP_LIB}
    ${CURL_LIB}
    m
    dl

    # 9. Qt and OpenGL
    Qt5::Widgets
    Qt5::OpenGL
    ${GLU_LIBRARY}
)
