set(OpenGL_GL_PREFERENCE GLVND)

# 1. Find Core OpenGL
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

# 2. Specifically look for the GLU library file
find_library(GLU_LIBRARY NAMES GLU glu)

# 3. Define the library
file(GLOB PICKING_SOURCES *.cpp *.h)
add_library(picking STATIC ${PICKING_SOURCES})

# 4. Include paths
target_include_directories(picking PRIVATE 
    ${OPENGL_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/shared
)
include_directories(/apps/spack-managed/gcc-11.3.1/mesa-glu-9.0.2-jt4lynuhi3xlsnsm6v77irx4g2dxujui/include)

# 5. Link using the variables we found
target_link_libraries(picking PRIVATE 
    ${OPENGL_LIBRARIES} # Usually libGL.so
    ${GLU_LIBRARY}      # This will be /usr/lib64/libGLU.so or similar
    Qt5::Widgets 
    Qt5::OpenGL
    shared
)
