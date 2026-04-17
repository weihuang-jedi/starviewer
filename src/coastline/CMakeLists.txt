file(GLOB_RECURSE SOURCES *.cpp *.h *.ui *.qrc)

find_library(NETCDF_CPP_LIB netcdf_c++4 REQUIRED)
find_library(NETCDF_C_LIB netcdf REQUIRED)

# 1. Find the OpenGL component of Qt5
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

# 2. Define the library
file(GLOB COASTLINE_SOURCES *.cpp *.h)
add_library(coastline STATIC ${COASTLINE_SOURCES})

# --- ADD THIS SECTION ---
# 3. Tell the compiler where to find netcdf.h
target_include_directories(coastline PRIVATE
    /usr/include
)
# ------------------------

# 4. Link the libraries
target_link_libraries(coastline PRIVATE
    ${NETCDF_CPP_LIB} 
    ${NETCDF_C_LIB}
    Qt5::Widgets
    Qt5::OpenGL
    shared
)
