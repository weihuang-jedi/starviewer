# 0. Find NetCDF using environment hints
set(NetCDF_INCLUDE_DIRS $ENV{NETCDF_C_ROOT}/include)
set(NetCDF_LIBRARIES $ENV{NETCDF_C_ROOT}/lib/libnetcdf.so)
find_path(NetCDF_INCLUDE_DIRS NAMES netcdf.h HINTS $ENV{NETCDF_C_ROOT}/include)
find_library(NetCDF_LIBRARIES NAMES netcdf HINTS $ENV{NETCDF_C_ROOT}/lib)

# 1. Find the required Qt5 components
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

# 2. Define the library
file(GLOB STATEBOUNDARY_SOURCES *.cpp *.h)
add_library(stateboundary STATIC ${STATEBOUNDARY_SOURCES})


# 3. Add include directories
# This ensures it can find headers from siblings if needed
target_include_directories(stateboundary PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/shared
    ${NetCDF_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../nclapp
)

# 4. Link the Qt5 OpenGL library
# Linking to Qt5::OpenGL automatically adds the necessary include paths to the compiler
target_link_libraries(stateboundary PRIVATE 
    ${NetCDF_LIBRARIES}
    Qt5::Widgets 
    Qt5::OpenGL
    shared
)
