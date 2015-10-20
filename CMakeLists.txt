## Project Colormaps for OpenCV

project( cvplot_project )
cmake_minimum_required( VERSION 2.6 )

if(NOT DEFINED WITH_QT)
set (WITH_QT ON)
endif()

# Find OpenCV:
find_package( OpenCV REQUIRED )
message("Found Opencv: ${OpenCV_VERSION}")

# Find Qt4
if (WITH_QT)
    message(STATUS "With Qt4")
    set(QTDIR $ENV{QT_DIR})
    find_package( Qt4 )
endif(WITH_QT)

# add library :
add_subdirectory("Library")
# add demo application :
add_subdirectory("Demo")
# add tests
add_subdirectory("Tests")


# install opencv library dlls :
if(WIN32)
    SET(OpenCV_BIN_DIR ${OpenCV_DIR}/bin)
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        INSTALL(DIRECTORY ${OpenCV_BIN_DIR}/ DESTINATION bin FILES_MATCHING PATTERN "*d.dll")
    else()
        INSTALL(DIRECTORY ${OpenCV_BIN_DIR}/ DESTINATION bin FILES_MATCHING PATTERN "*.dll" PATTERN "*d.dll" EXCLUDE)
    endif()
endif(WIN32)

