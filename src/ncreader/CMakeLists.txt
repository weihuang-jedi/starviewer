file(GLOB_RECURSE SOURCES *.cpp *.h *.ui *.qrc)

# 1. Find the OpenGL component of Qt5
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

# 2. Define the library
file(GLOB COASTLINE_SOURCES *.cpp *.h)
add_library(ncreader STATIC ${COASTLINE_SOURCES})

# --- ADD THIS SECTION ---
# 3. Tell the compiler where to find netcdf.h
target_include_directories(ncreader PRIVATE
    ${NetCDF_INCLUDE_DIRS}
)
# ------------------------

# 4. Link the libraries
target_link_libraries(ncreader PRIVATE
    ${NetCDF_LIBRARIES}
    Qt5::Widgets
    Qt5::OpenGL
    shared
)
