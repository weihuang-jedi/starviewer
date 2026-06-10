file(GLOB_RECURSE SOURCES *.cpp *.h *.ui *.qrc)

# Find Core OpenGL
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

# Create a library for this module
add_library(control STATIC ${SOURCES})

# Allow other modules to find these headers
target_include_directories(control PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/geometry
    ${CMAKE_SOURCE_DIR}/shared
    ${CMAKE_SOURCE_DIR}/leveler
    ${CMAKE_SOURCE_DIR}/spreadsheet
    ${CMAKE_SOURCE_DIR}/coastline
    ${CMAKE_SOURCE_DIR}/marchingCube
    ${CMAKE_SOURCE_DIR}/ncreader
    ${CMAKE_SOURCE_DIR}/contour
    ${CMAKE_SOURCE_DIR}/view
)

# Link Qt5
target_link_libraries(control PUBLIC Qt5::Core Qt5::Widgets Qt5::OpenGL Qt5::Gui)
