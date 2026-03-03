file(GLOB_RECURSE SOURCES *.cpp *.h *.ui *.qrc)

# Create a library for this module
add_library(coastline STATIC ${SOURCES})

# Allow other modules to find these headers
target_include_directories(coastline PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link Qt5
target_link_libraries(coastline PUBLIC Qt5::Core Qt5::Widgets Qt5::Gui)
