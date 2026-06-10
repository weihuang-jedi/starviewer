file(GLOB_RECURSE SOURCES *.cpp *.h *.ui *.qrc)

# Find Core OpenGL
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

# Create a library for this module
add_library(contour STATIC ${SOURCES})

# Allow other modules to find these headers
target_include_directories(contour PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${NetCDF_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/geometry
    ${CMAKE_SOURCE_DIR}/shared
)

# Link Qt5
target_link_libraries(contour PUBLIC
    ${NetCDF_LIBRARIES}
    Qt5::Core
    Qt5::OpenGL
    Qt5::Widgets
    Qt5::Gui
)
