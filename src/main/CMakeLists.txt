file(GLOB_RECURSE SOURCES *.cpp *.h *.ui *.qrc)

set(OpenGL_GL_PREFERENCE GLVND)

# Find OpenMP
find_package(OpenMP REQUIRED)

# 1. Find Core Qt5
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
# 2. Find the specific OpenGL component explicitly
find_package(Qt5OpenGL REQUIRED)

# Specifically look for the GLU library file
find_library(GLU_LIBRARY NAMES GLU glu)

# Manually search for the directory containing GL/glu.h
#find_path(GLU_INCLUDE_DIR NAMES GL/glu.h
#    PATHS /apps/spack-managed/gcc-11.3.1/mesa-glu-9.0.2-jt4lynuhi3xlsnsm6v77irx4g2dxujui/include
#    PATH_SUFFIXES GL
#)

# Create a library for this module
# add_library(starviewer STATIC ${SOURCES})
add_executable(starviewer
    main.cpp
    mainwindow.cpp
    mpidemoparser.cpp
    ufsparser.cpp
    ufsincrparser.cpp
    ufsmom6parser.cpp
    mpasparser.cpp
    # ... other files
    ../ncreader/ncreader.cpp
    ../geometry/geometry.cpp
    ../spreadsheet/spreadsheet.cpp
    # ADD YOUR NEW NETCDF READERS HERE:
    ../ncreader/ncbasereader.cpp
    ../ncreader/mpasdatareader.cpp
    ../ncreader/mpasstaticreader.cpp
    ../ncreader/ufsmom6reader.cpp
    ../ncreader/ufsgridreader.cpp
    ../ncreader/ufsincrementreader.cpp
)

# Allow other modules to find these headers
target_include_directories(starviewer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${GLU_INCLUDE_DIR}
    ${GLU_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/geometry
    ${CMAKE_SOURCE_DIR}/shared
    ${CMAKE_SOURCE_DIR}/coastline
    ${CMAKE_SOURCE_DIR}/topography
    ${CMAKE_SOURCE_DIR}/ncreader
    ${CMAKE_SOURCE_DIR}/stateboundary
    ${CMAKE_SOURCE_DIR}/contour
    ${CMAKE_SOURCE_DIR}/control
    ${CMAKE_SOURCE_DIR}/view
    ${CMAKE_SOURCE_DIR}/windvector
    ${CMAKE_SOURCE_DIR}/marchingCube
    ${CMAKE_SOURCE_DIR}/translator
    ${CMAKE_SOURCE_DIR}/trajectory
    ${CMAKE_SOURCE_DIR}/widget
    ${CMAKE_SOURCE_DIR}/lic
    ${CMAKE_SOURCE_DIR}/ufs
    ${CMAKE_SOURCE_DIR}/ufsincr
    ${CMAKE_SOURCE_DIR}/ufsmom6
    ${CMAKE_SOURCE_DIR}/mpas
    ${CMAKE_SOURCE_DIR}/mpidemo
    ${NetCDF_INCLUDE_DIRS}
)

target_link_libraries(starviewer PRIVATE
    # 1. High-level UI Modules (Must come before main/executable)
    ncreader
    control
    translator      # Fixes TestTranslator and MPITranslator
    widget          # Fixes UI components

    # 2. Logic and Data Modules
    mpidemo
    ufs
    ufsincr
    ufsmom6
    mpas
    view
    contour
    shared
    geometry
    trajectory
    windvector
    topography
    coastline
    marchingCube
    leveler
    lic
    picking
    spreadsheet
    stateboundary
    control
    view
    leveler

    -Wl,--start-group
    -Wl,--end-group

    # 4. System Graphics and Fortran Runtimes
    X11 Xext Xrender
    gfortran
    OpenMP::OpenMP_CXX

    # 5. Data Formats and Math
    ${NetCDF_LIBRARIES}
    ${HDF5_HL_LIB}
    ${HDF5_LIB}
    ${ZLIB_LIBRARIES}
    gfortran
    yaml-cpp
    curl
    m
    dl

    # 6. Qt Framework
    Qt5::Widgets
    Qt5::OpenGL
)

# 7. Add the Multiple Definition Flag
set_target_properties(starviewer PROPERTIES
    LINK_FLAGS "-Wl,--allow-multiple-definition"
)

