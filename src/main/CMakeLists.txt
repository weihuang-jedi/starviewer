# main/CMakeLists.txt
file(GLOB_RECURSE SOURCES *.cpp *.h *.ui *.qrc)
add_executable(starviewer ${SOURCES})

# Tell CMake where the NCL .so/.a files live
target_link_directories(starviewer PRIVATE ${NCL_INSTALL_DIR}/lib)

# Link internal sub-modules
target_link_libraries(starviewer PRIVATE
    shared
    geometry
    nclapp
    # Add other internal libs...
)

# Link NCL and its standard dependency stack
# Note: The order here matters for static linking (.a files)
target_link_libraries(starviewer PRIVATE
    nclapi ncarg ncarg_gks ncarg_c
    netcdf hdf5_hl hdf5 z
    Qt5::Widgets Qt5::Gui Qt5::Core
)

install(TARGETS starviewer DESTINATION bin)
