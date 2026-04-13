file(GLOB_RECURSE SOURCES *.cpp *.h *.ui *.qrc)

# 1. Properly find OpenGL (this sets OPENGL_INCLUDE_DIR and OPENGL_LIBRARIES)
set(OpenGL_GL_PREFERENCE GLVND)
#find_package(OpenGL REQUIRED)

# Qt5 settings
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

# Specifically look for the GLU library file
#find_library(GLU_LIBRARY NAMES GLU glu)

# Manually search for the directory containing GL/glu.h
#find_path(GLU_INCLUDE_DIR NAMES GL/glu.h
#    PATHS /apps/spack-managed/gcc-11.3.1/mesa-glu-9.0.2-jt4lynuhi3xlsnsm6v77irx4g2dxujui/include
#    PATH_SUFFIXES GL
#)
#
#message(STATUS "DEBUG: OpenGL Include Dir is: ${OPENGL_INCLUDE_DIR}")
#message(STATUS "DEBUG: GLU Library is: ${GLU_LIBRARY}")
#message(STATUS "FOUND GLU AT: ${GLU_INCLUDE_DIR}")

# Create a library
add_library(translator STATIC ${SOURCES})

# 2. Add Include Directories - IMPORTANT: Add ${OPENGL_INCLUDE_DIR}
target_include_directories(translator PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${GLU_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/geometry
    ${CMAKE_SOURCE_DIR}/shared
    ${CMAKE_SOURCE_DIR}/leveler
    ${CMAKE_SOURCE_DIR}/spreadsheet
    ${CMAKE_SOURCE_DIR}/coastline
    ${CMAKE_SOURCE_DIR}/nclapp
    ${CMAKE_SOURCE_DIR}/contour
    ${CMAKE_SOURCE_DIR}/control
    ${CMAKE_SOURCE_DIR}/ncreader
    ${CMAKE_SOURCE_DIR}/marchingCube
    ${CMAKE_SOURCE_DIR}/view
    ${NetCDF_INCLUDE_DIRS}
)

# 3. Link libraries
target_link_libraries(translator PUBLIC
    ${NetCDF_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${GLU_LIBRARY}
    Qt5::Core
    Qt5::Widgets
    Qt5::OpenGL
    Qt5::Gui
)
