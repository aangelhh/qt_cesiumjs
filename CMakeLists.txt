cmake_minimum_required(VERSION 3.21)

project(qt_test VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui)
find_package(Qt6 QUIET COMPONENTS WebEngineWidgets WebEngineCore WebChannel Positioning)

qt_standard_project_setup()

qt_add_executable(appqt_test
    main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
)

target_link_libraries(appqt_test
    PRIVATE
        Qt6::Widgets
        Qt6::Gui
)

if(
    Qt6WebEngineWidgets_FOUND
    AND Qt6WebEngineCore_FOUND
    AND Qt6WebChannel_FOUND
    AND Qt6Positioning_FOUND
)
    target_link_libraries(appqt_test PRIVATE
        Qt6::WebEngineWidgets
        Qt6::WebChannel
    )
    target_compile_definitions(appqt_test PRIVATE QT_CESIUMJS_WEBENGINE_AVAILABLE=1)
endif()

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
