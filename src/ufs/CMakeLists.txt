file(GLOB_RECURSE SOURCES *.cpp *.h *.ui *.qrc)

set(OpenGL_GL_PREFERENCE GLVND)

# 1. Find Core Qt5
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
# 2. Find the specific OpenGL component explicitly
find_package(Qt5OpenGL REQUIRED)

# Specifically look for the GLU library file
find_library(GLU_LIBRARY NAMES GLU glu)

# Manually search for the directory containing GL/glu.h
find_path(GLU_INCLUDE_DIR NAMES GL/glu.h
    PATHS /apps/spack-managed/gcc-11.3.1/mesa-glu-9.0.2-jt4lynuhi3xlsnsm6v77irx4g2dxujui/include
    PATH_SUFFIXES GL
)

# Create a library for this module
add_library(ufs STATIC ${SOURCES})

# Allow other modules to find these headers
target_include_directories(ufs PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${GLU_INCLUDE_DIR}
    ${GLU_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/geometry
    ${CMAKE_SOURCE_DIR}/shared
    ${CMAKE_SOURCE_DIR}/leveler
    ${CMAKE_SOURCE_DIR}/spreadsheet
    ${CMAKE_SOURCE_DIR}/coastline
    ${CMAKE_SOURCE_DIR}/topography
    ${CMAKE_SOURCE_DIR}/widget
    ${CMAKE_SOURCE_DIR}/stateboundary
    ${CMAKE_SOURCE_DIR}/contour
    ${CMAKE_SOURCE_DIR}/control
    ${CMAKE_SOURCE_DIR}/ncreader
    ${CMAKE_SOURCE_DIR}/view
    ${CMAKE_SOURCE_DIR}/windvector
    ${CMAKE_SOURCE_DIR}/marchingCube
    ${CMAKE_SOURCE_DIR}/translator
    ${CMAKE_SOURCE_DIR}/trajectory
    ${CMAKE_SOURCE_DIR}/lic
    ${NetCDF_INCLUDE_DIRS}
)

# Link Qt5
target_link_libraries(ufs PUBLIC
    ${OPENGL_LIBRARIES}
    ${NetCDF_LIBRARIES}
    ${GLU_LIBRARY}
    Qt5::Core
    Qt5::Widgets
    Qt5::OpenGL
    Qt5::Gui
)
