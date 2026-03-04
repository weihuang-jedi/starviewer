file(GLOB_RECURSE SOURCES *.cpp *.h *.ui *.qrc)

# Find Core OpenGL
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

set(OpenGL_GL_PREFERENCE GLVND)
# NetCDF settings
set(NetCDF_INCLUDE_DIRS $ENV{NETCDF_C_ROOT}/include)
set(NetCDF_LIBRARIES $ENV{NETCDF_C_ROOT}/lib/libnetcdf.so)

# Qt5 settings
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

# Specifically look for the GLU library file
find_library(GLU_LIBRARY NAMES GLU glu)

# Manually search for the directory containing GL/glu.h
find_path(GLU_INCLUDE_DIR NAMES GL/glu.h
    PATHS /apps/spack-managed/gcc-11.3.1/mesa-glu-9.0.2-jt4lynuhi3xlsnsm6v77irx4g2dxujui/include
    PATH_SUFFIXES GL
)

# message(STATUS "DEBUG: OpenGL Include Dir is: ${OPENGL_INCLUDE_DIR}")
# message(STATUS "DEBUG: GLU Library is: ${GLU_LIBRARY}")
# message(STATUS "FOUND GLU AT: ${GLU_INCLUDE_DIR}")

# Create a library for this module
add_library(widget STATIC ${SOURCES})

# Allow other modules to find these headers
target_include_directories(widget PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${GLU_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/geometry
    ${CMAKE_SOURCE_DIR}/shared
    ${CMAKE_SOURCE_DIR}/coastline
    ${CMAKE_SOURCE_DIR}/nclapp
    ${CMAKE_SOURCE_DIR}/contour
    ${CMAKE_SOURCE_DIR}/control
    ${CMAKE_SOURCE_DIR}/marchingCube
    ${CMAKE_SOURCE_DIR}/view
    ${CMAKE_SOURCE_DIR}/translator
    ${NetCDF_INCLUDE_DIRS}
)

# Link Qt5
target_link_libraries(widget PUBLIC
    ${NetCDF_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${GLU_LIBRARY}
    Qt5::Core
    Qt5::Widgets
    Qt5::OpenGL
    Qt5::Gui
)
