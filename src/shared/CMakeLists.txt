file(GLOB_RECURSE SOURCES *.cpp *.h *.f *.ui *.qrc)

set(NetCDF_INCLUDE_DIRS $ENV{NETCDF_C_ROOT}/include)
set(NetCDF_LIBRARIES $ENV{NETCDF_C_ROOT}/lib/libnetcdf.so)
find_path(NetCDF_INCLUDE_DIRS NAMES netcdf.h)
find_library(NetCDF_LIBRARIES NAMES netcdf)

# 1. Remove "OpenGL" from the find_package list
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# 2. Create the library FIRST before linking
add_library(shared STATIC ${SOURCES})

# 3. Allow other modules to find headers
target_include_directories(shared PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
        ${NetCDF_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/ncreader
)

# 4. Link only the available modules
target_link_libraries(shared 
    PUBLIC 
        ${NetCDF_LIBRARIES}
        Qt5::Core 
        Qt5::Widgets 
        Qt5::Gui
)

include_directories(/apps/spack-managed/gcc-11.3.1/mesa-glu-9.0.2-jt4lynuhi3xlsnsm6v77irx4g2dxujui/include)
