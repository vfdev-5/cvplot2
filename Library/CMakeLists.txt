

project( cvplot )

## include & link to OpenCV :
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIB_DIR})
link_libraries(${OpenCV_LIBS})

## include & link to Qt if defined:
if(WITH_QT)
    include(${QT_USE_FILE})
    link_libraries(${QT_LIBRARIES})
    add_definitions( -DHAS_QT4 )
endif()


## include project source directories:
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

## source files:
set (SRC_FILES "src/plot.cpp")
set (INC_FILES_TO_INSTALL "include/cvplot.h")

if (QT4_FOUND)
    set (SRC_FILES "${SRC_FILES};src/plot_qt4.cpp")
    set (INC_FILES_TO_INSTALL "${INC_FILES_TO_INSTALL};include/cvplot_qt4.h")
endif()
set (INC_FILES "${INC_FILES_TO_INSTALL};include/plot.h")

## create shared library :
add_definitions(-DCVPLOT_EXPORTS)
add_library( ${PROJECT_NAME} SHARED ${SRC_FILES} ${INC_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ".d")

## installation :
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
install(FILES ${INC_FILES_TO_INSTALL} DESTINATION include)
