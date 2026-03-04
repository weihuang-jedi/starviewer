file(GLOB_RECURSE SOURCES *.cpp *.h *.ui *.qrc)

set(OpenGL_GL_PREFERENCE GLVND)

# 1. Find Core Qt5
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
# 2. Find the specific OpenGL component explicitly
find_package(Qt5OpenGL REQUIRED)

# Create a library for this module
add_library(lic STATIC ${SOURCES})

# Allow other modules to find these headers
target_include_directories(lic PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/shared
)

# Link Qt5
target_link_libraries(lic PUBLIC
    ${OPENGL_LIBRARIES}
    Qt5::Core
    Qt5::Widgets
    Qt5::OpenGL
    Qt5::Gui
)
