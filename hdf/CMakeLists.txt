file(GLOB_RECURSE SOURCES *.cpp *.h *.ui *.qrc)

# Create a library for this module
add_library(hdf STATIC ${SOURCES})

# Allow other modules to find these headers
target_include_directories(hdf PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link Qt5
target_link_libraries(hdf PUBLIC Qt5::Core Qt5::Widgets Qt5::Gui)
