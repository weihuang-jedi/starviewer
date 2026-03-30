cmake_minimum_required(VERSION 3.10)
project(starviewer VERSION 1.0 LANGUAGES CXX Fortran)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)

# Find Qt5
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui)

# Note: In some systems, it's 'Qt5OpenGL', not just 'OpenGL' inside Widgets.
#find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)
#target_link_libraries(geometry PRIVATE Qt5::Widgets Qt5::OpenGL)

# Setup NCL Paths from Environment
if(DEFINED ENV{NCL_ROOT})
    set(NCL_INSTALL_DIR $ENV{NCL_ROOT})
else()
    # Fallback to your specific path if module isn't loaded
   #set(NCL_INSTALL_DIR "/apps/spack-managed/gcc-11.3.1/ncl-6.6.2-zlkxvxdkwsncu7cj2v5m2nnnwoma4sx6")
    set(NCL_INSTALL_DIR "/apps/spack-2024-12/linux-rocky9-x86_64/gcc-11.4.1/ncl-6.6.2-uq4jey5bnminpqoithv7hsx4svmmu4mn")
endif()

message(STATUS "Using NCL from: ${NCL_INSTALL_DIR}")

# Global Includes for NCL
include_directories(${NCL_INSTALL_DIR}/include)

# Setup Paths
#set(CMAKE_INSTALL_PREFIX "/work2/noaa/epic/weihuang/nv/starviewer/install")
set(CMAKE_INSTALL_PREFIX "/scratch4/NAGAPE/epic/Wei.Huang/src/nv/starviewer/install")

# Subdirectories
add_subdirectory(geometry)
add_subdirectory(shared)
add_subdirectory(nclapp)
add_subdirectory(ncreader)
add_subdirectory(leveler)
add_subdirectory(spreadsheet)
add_subdirectory(coastline)
add_subdirectory(coastline2)
add_subdirectory(stateboundary)
add_subdirectory(topography)
add_subdirectory(picking)
add_subdirectory(contour)
add_subdirectory(marchingCube)
add_subdirectory(marchTetrahedron)
add_subdirectory(view)
add_subdirectory(control)
add_subdirectory(translator)
add_subdirectory(widget)
add_subdirectory(windvector)
add_subdirectory(trajectory)
add_subdirectory(lic)
add_subdirectory(wrf)
add_subdirectory(ufs)
add_subdirectory(mpas)
add_subdirectory(camse)
add_subdirectory(pop)
add_subdirectory(hdf)
add_subdirectory(test)
add_subdirectory(mpidemo)
add_subdirectory(main)
