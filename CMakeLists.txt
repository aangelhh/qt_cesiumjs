cmake_minimum_required(VERSION 3.21)

project(qt_test VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
    find_program(APPLE_LIBTOOL libtool REQUIRED)
    set(CMAKE_C_ARCHIVE_CREATE
        "${APPLE_LIBTOOL} -static -no_warning_for_no_symbols -o <TARGET> <LINK_FLAGS> <OBJECTS>")
    set(CMAKE_CXX_ARCHIVE_CREATE
        "${APPLE_LIBTOOL} -static -no_warning_for_no_symbols -o <TARGET> <LINK_FLAGS> <OBJECTS>")
    set(CMAKE_C_ARCHIVE_FINISH "")
    set(CMAKE_CXX_ARCHIVE_FINISH "")
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGL OpenGLWidgets Gui)

qt_standard_project_setup()

add_subdirectory(cesium)

qt_add_executable(appqt_test
    main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/CesiumRuntime.cpp
    src/CesiumRuntime.h
    src/CesiumViewport.cpp
    src/CesiumViewport.h
    src/NullPrepareRendererResources.h
)

target_link_libraries(appqt_test
    PRIVATE
        Qt6::Widgets
        Qt6::OpenGL
        Qt6::OpenGLWidgets
        Qt6::Gui
        Cesium3DTilesSelection
        CesiumAsync
        CesiumCurl
        CesiumUtility
        CesiumIonClient
        CesiumRasterOverlays
        CesiumGeospatial
)

target_include_directories(appqt_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_target_properties(appqt_test PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS appqt_test
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
