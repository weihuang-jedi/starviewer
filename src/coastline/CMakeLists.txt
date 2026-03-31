file(GLOB_RECURSE SOURCES *.cpp *.h *.ui *.qrc)

# 0. Find NetCDF using environment hints
find_path(NetCDF_INCLUDE_DIRS NAMES netcdf.h HINTS $ENV{NETCDF_C_ROOT}/include)
find_library(NetCDF_LIBRARIES NAMES netcdf HINTS $ENV{NETCDF_C_ROOT}/lib)

# 1. Find the OpenGL component of Qt5
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

# 2. Define the library
file(GLOB COASTLINE_SOURCES *.cpp *.h)
add_library(coastline STATIC ${COASTLINE_SOURCES})

# --- ADD THIS SECTION ---
# 3. Tell the compiler where to find netcdf.h
target_include_directories(coastline PRIVATE
    ${NetCDF_INCLUDE_DIRS}
  # ${CMAKE_CURRENT_SOURCE_DIR}/../nclapp  # Path to nclapp headers
)
# ------------------------

# 4. Link the libraries
target_link_libraries(coastline PRIVATE
    ${NetCDF_LIBRARIES}
    Qt5::Widgets
    Qt5::OpenGL
    shared
)
