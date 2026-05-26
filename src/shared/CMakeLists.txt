file(GLOB_RECURSE SOURCES *.cpp *.h *.f *.ui *.qrc)

find_library(NETCDF_CPP_LIB netcdf_c++4 REQUIRED)
find_library(NETCDF_C_LIB netcdf REQUIRED)

# 1. Remove "OpenGL" from the find_package list
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(yaml-cpp REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# 2. Create the library FIRST before linking
add_library(shared STATIC ${SOURCES})

# Link yaml-cpp to the shared library
target_link_libraries(shared PUBLIC yaml-cpp)

# 3. Allow other modules to find headers
target_include_directories(shared PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
        /usr/include
        ${CMAKE_SOURCE_DIR}/ncreader
)

# 4. Link only the available modules
target_link_libraries(shared 
    PUBLIC 
        ${NETCDF_CPP_LIB}
        ${NETCDF_C_LIB}
        Qt5::Core 
        Qt5::Widgets 
        Qt5::Gui
)

