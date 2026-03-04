import os

# The list from your .pro file
subdirs = [
    "geometry", "shared", "nclapp", "coastline", "coastline2", 
    "stateboundary", "topography", "picking", "contour", "marchingCube", 
    "marchTetrahedron", "view", "control", "translator", "widget", 
    "windvector", "trajectory", "lic", "wrf", "mpas", "camse", 
    "pop", "hdf", "test", "mpidemo", "main"
]

# 1. Create the Root CMakeLists.txt
sub_dirs_str = "\n".join(["add_subdirectory({})".format(d) for d in subdirs])

root_template = """cmake_minimum_required(VERSION 3.10)
project(NVProject VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)

# Find Qt5
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui)

# Setup Paths
set(NCL_DIR "/Users/huangwei/ncl/NCLDEV/lib")
set(CMAKE_INSTALL_PREFIX "/usr/local")

# Subdirectories
{}
"""

with open("CMakeLists.txt", "w") as f:
    f.write(root_template.format(sub_dirs_str))

# 2. Create individual subdirectory CMakeLists.txt
for d in subdirs:
    if not os.path.exists(d):
        os.makedirs(d)
    
    if d == "main":
        content = """file(GLOB_RECURSE SOURCES *.cpp *.h *.ui *.qrc)
add_executable(main ${SOURCES})

# Ensure NCL libraries are found
link_directories(${NCL_DIR})

# Link internal libs and Qt5
target_link_libraries(main PRIVATE 
    shared 
    geometry 
    nclapp
    Qt5::Widgets 
    Qt5::Gui 
    Qt5::Core
)

install(TARGETS main DESTINATION bin)
"""
    else:
        content = """file(GLOB_RECURSE SOURCES *.cpp *.h *.ui *.qrc)

# Create a library for this module
add_library({0} STATIC ${{SOURCES}})

# Allow other modules to find these headers
target_include_directories({0} PUBLIC ${{CMAKE_CURRENT_SOURCE_DIR}})

# Link Qt5
target_link_libraries({0} PUBLIC Qt5::Core Qt5::Widgets Qt5::Gui)
""".format(d)
    
    with open(os.path.join(d, "CMakeLists.txt"), "w") as f:
        f.write(content)

print("Generated CMakeLists.txt files successfully.")

