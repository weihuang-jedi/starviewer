# 1. Find NetCDF using CMake's search logic
set(NetCDF_INCLUDE_DIRS $ENV{NETCDF_C_ROOT}/include)
set(NetCDF_LIBRARIES $ENV{NETCDF_C_ROOT}/lib/libnetcdf.so)
find_path(NetCDF_INCLUDE_DIRS NAMES netcdf.h)
find_library(NetCDF_LIBRARIES NAMES netcdf)

# 2. Define the library
file(GLOB NCLAPP_SOURCES *.cpp *.h)
add_library(nclapp STATIC ${NCLAPP_SOURCES})

# 3. Add the include directory (The fix for your fatal error)
target_include_directories(nclapp PUBLIC ${NetCDF_INCLUDE_DIRS})

# 4. Link the actual NetCDF library
target_link_libraries(nclapp PRIVATE ${NetCDF_LIBRARIES} Qt5::Widgets shared)
