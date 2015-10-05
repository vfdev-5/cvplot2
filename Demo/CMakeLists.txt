
project( demo )

file(GLOB SRC_FILES "*.cpp")

## include & link to OpenCV :
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIB_DIR})
link_libraries(${OpenCV_LIBS})

## include & link to Qt if defined:
if(QT4_FOUND)
    include(${QT_USE_FILE})
    link_libraries(${QT_LIBRARIES})
endif()


## include & link to cvplot library
include_directories(${CMAKE_SOURCE_DIR}/Library/include)
include_directories(${CMAKE_BINARY_DIR}/Library/)
link_libraries("cvplot")

## create application
add_executable( ${PROJECT_NAME} ${SRC_FILES} )
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ".d")

## install application
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION Bin ARCHIVE DESTINATION Lib LIBRARY DESTINATION Lib)
