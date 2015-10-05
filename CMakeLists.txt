## Project Colormaps for OpenCV

project( cvplot_project )
cmake_minimum_required( VERSION 2.6 )

# Find OpenCV:
find_package( OpenCV REQUIRED )

# Find Qt4
set(QTDIR $ENV{QT_DIR})
find_package( Qt4 )

# add library :
add_subdirectory("Library")
# add demo application :
add_subdirectory("Demo")

# install opencv library dlls :
if(WIN32)
    SET(OpenCV_BIN_DIR ${OpenCV_DIR}/bin)
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        INSTALL(DIRECTORY ${OpenCV_BIN_DIR}/ DESTINATION bin FILES_MATCHING PATTERN "*d.dll")
    else()
        INSTALL(DIRECTORY ${OpenCV_BIN_DIR}/ DESTINATION bin FILES_MATCHING PATTERN "*.dll" PATTERN "*d.dll" EXCLUDE)
    endif()
endif(WIN32)

