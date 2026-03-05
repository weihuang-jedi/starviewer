file(GLOB_RECURSE SOURCES *.cpp *.h *.ui *.qrc)

# Find Core OpenGL
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

# Create a library for this module
add_library(spreadsheet STATIC ${SOURCES})

# Allow other modules to find these headers
target_include_directories(spreadsheet PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/shared
    ${CMAKE_SOURCE_DIR}/geometry
)

# Link Qt5
target_link_libraries(spreadsheet PUBLIC Qt5::Core Qt5::OpenGL Qt5::Widgets Qt5::Gui)
