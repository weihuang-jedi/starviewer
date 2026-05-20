cmake_minimum_required(VERSION 3.10)
project(starviewer VERSION 1.0 LANGUAGES CXX Fortran)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)

find_package(GDAL REQUIRED)
# Add this so every sub-directory knows where GDAL is regardless of linking
include_directories(${GDAL_INCLUDE_DIRS})

# Find Qt5
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui)

# Note: In some systems, it's 'Qt5OpenGL', not just 'OpenGL' inside Widgets.
#find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)
#target_link_libraries(geometry PRIVATE Qt5::Widgets Qt5::OpenGL)

# Subdirectories
add_subdirectory(geometry)
add_subdirectory(shared)
add_subdirectory(ncreader)
add_subdirectory(leveler)
add_subdirectory(spreadsheet)
add_subdirectory(coastline)
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
add_subdirectory(ufs)
add_subdirectory(ufsincr)
add_subdirectory(mpidemo)
add_subdirectory(main)
