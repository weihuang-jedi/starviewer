# 1. Define sources
file(GLOB COASTLINE_SOURCES *.cpp *.h)

# 2. Find Dependencies
find_package(GDAL REQUIRED)
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

# 3. Create the target FIRST
add_library(coastline STATIC ${COASTLINE_SOURCES})

# 4. Now configure the target (using PUBLIC so 'view' and 'control' can see GDAL)
target_include_directories(coastline PUBLIC 
    ${GDAL_INCLUDE_DIRS}
)

# 5. Link libraries
# Note: Use PUBLIC for GDAL so other modules that include coastline.h 
# automatically find the GDAL headers/libs.
target_link_libraries(coastline PUBLIC 
    ${GDAL_LIBRARIES}
    Qt5::Widgets
    Qt5::OpenGL
)

target_link_libraries(coastline PRIVATE
    shared
)
