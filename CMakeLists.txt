cmake_minimum_required(VERSION 3.21)

project(qttest VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui)
find_package(Qt6 QUIET COMPONENTS WebEngineWidgets WebEngineCore WebChannel Positioning)

qt_standard_project_setup()

option(QTTEST_ENABLE_JSBSIM "Enable JSBSim flight dynamics backend when available" ON)

if(QTTEST_ENABLE_JSBSIM AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/jsbsim/CMakeLists.txt")
    set(BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(BUILD_PYTHON_MODULE OFF CACHE BOOL "" FORCE)
    set(BUILD_JULIA_PACKAGE OFF CACHE BOOL "" FORCE)
    set(BUILD_MATLAB_SFUNCTION OFF CACHE BOOL "" FORCE)
    add_subdirectory(Dependencies/jsbsim EXCLUDE_FROM_ALL)
endif()

qt_add_executable(qttest
    main.cpp
    src/AssignTaskDialog.cpp
    src/AssignTaskDialog.h
    src/AddEntityDialog.cpp
    src/AddEntityDialog.h
    src/EntityDetailsDialog.cpp
    src/EntityDetailsDialog.h
    src/infrastructure/CesiumScenePage.cpp
    src/infrastructure/CesiumScenePage.h
    src/MainWindow.cpp
    src/MainWindow.h
    src/MainWindow.ui
    src/infrastructure/MapBridge.h
    src/infrastructure/DisEntityCatalog.cpp
    src/infrastructure/DisEntityCatalog.h
    src/infrastructure/ModelCatalog.cpp
    src/infrastructure/ModelCatalog.h
    src/application/ScenarioState.cpp
    src/application/ScenarioState.h
    src/application/Command.cpp
    src/application/Command.h
    src/application/Event.h
    src/application/EventBus.h
    src/application/SimulationEngine.cpp
    src/application/SimulationEngine.h
    src/application/FlightDynamicsEngine.cpp
    src/application/FlightDynamicsEngine.h
    src/application/SensorEngine.cpp
    src/application/SensorEngine.h
    src/domain/BombReleaseGate.cpp
    src/domain/BombReleaseGate.h
    src/domain/Entity.h
    src/domain/GeoMath.cpp
    src/domain/GeoMath.h
    src/domain/Sensor.h
    src/domain/Task.cpp
    src/domain/Task.h
    src/presentation/EntityHomePositionTracker.cpp
    src/presentation/EntityHomePositionTracker.h
    src/presentation/EntityTextFormatter.cpp
    src/presentation/EntityTextFormatter.h
    src/presentation/EntityVisualStateManager.cpp
    src/presentation/EntityVisualStateManager.h
)

target_link_libraries(qttest
    PRIVATE
        Qt6::Widgets
        Qt6::Gui
)

if(TARGET libJSBSim)
    target_link_libraries(qttest PRIVATE libJSBSim)
    target_compile_definitions(qttest PRIVATE QTTEST_HAS_JSBSIM=1)
endif()

if(
    Qt6WebEngineWidgets_FOUND
    AND Qt6WebEngineCore_FOUND
    AND Qt6WebChannel_FOUND
    AND Qt6Positioning_FOUND
)
    target_link_libraries(qttest PRIVATE
        Qt6::WebEngineWidgets
        Qt6::WebChannel
    )
    target_compile_definitions(qttest PRIVATE QT_CESIUMJS_WEBENGINE_AVAILABLE=1)
endif()

target_include_directories(qttest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(qttest PRIVATE
    QTTEST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

set_target_properties(qttest PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS qttest
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ---------------------------------------------------------------------------
# Warning flags
# ---------------------------------------------------------------------------
if(MSVC)
    target_compile_options(qttest PRIVATE /W4 /permissive-)
else()
    target_compile_options(qttest PRIVATE
        -Wall -Wextra -Wpedantic
        -Wno-unused-parameter
    )
endif()

# ---------------------------------------------------------------------------
# Tests (opt-in)
# ---------------------------------------------------------------------------
option(QTTEST_BUILD_TESTS "Build qttest unit tests" ON)
if(QTTEST_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
