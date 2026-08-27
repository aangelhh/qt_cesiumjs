cmake_minimum_required(VERSION 3.21)

project(qttest VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui)
find_package(Qt6 QUIET COMPONENTS WebEngineWidgets WebEngineCore WebChannel Positioning)

option(
    QTTEST_ENABLE_ROS2_TELEMETRY
    "Publish kinematics telemetry directly through ROS 2/DDS"
    OFF
)
set(QTTEST_ROS2_BACKEND_SOURCE
    src/infrastructure/Ros2TelemetryBackendStub.cpp
)
if(QTTEST_ENABLE_ROS2_TELEMETRY)
    find_package(rclcpp REQUIRED)
    find_package(qttest_interfaces REQUIRED)
    if(APPLE)
        # RoboStack exports rclcpp with rosidl_generator_py libraries in its
        # transitive link interface. Load libpython so those dylibs can resolve
        # their Python runtime symbols when a native C++ process starts.
        find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
    endif()
    set(QTTEST_ROS2_BACKEND_SOURCE
        src/infrastructure/Ros2TelemetryBackendNative.cpp
    )
endif()

option(QTTEST_ENABLE_MODERN_PFD "Enable the embedded modern kinematics PFD" ON)
if(QTTEST_ENABLE_MODERN_PFD)
    find_package(Qt6 QUIET COMPONENTS Quick QuickWidgets Qml)
    if(Qt6Quick_FOUND AND Qt6QuickWidgets_FOUND AND Qt6Qml_FOUND)
        set(KINEMATICS_PFD_RESOURCE
            ${CMAKE_CURRENT_SOURCE_DIR}/src/presentation/KinematicsPfd.qrc
        )
        set(QTTEST_HAS_MODERN_PFD TRUE)
    else()
        message(WARNING "Modern kinematics PFD disabled: Qt6 Quick/QuickWidgets/Qml not available")
    endif()
endif()

option(QTTEST_ENABLE_QFLIGHTINSTRUMENTS "Enable the embedded QFlightInstruments cockpit" ON)
set(QTTEST_QFLIGHTINSTRUMENTS_GIT_TAG
    "b1401ba9f96cc954d44d59559efebad2ea36e3f9"
    CACHE STRING "Pinned QFlightInstruments revision")

if(QTTEST_ENABLE_QFLIGHTINSTRUMENTS)
    find_package(Qt6 QUIET COMPONENTS Svg SvgWidgets)
    if(Qt6Svg_FOUND AND Qt6SvgWidgets_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            qflightinstruments
            GIT_REPOSITORY https://github.com/marek-cel/QFlightInstruments.git
            GIT_TAG ${QTTEST_QFLIGHTINSTRUMENTS_GIT_TAG}
            GIT_SHALLOW FALSE
            SOURCE_SUBDIR qttest-no-upstream-build
        )
        FetchContent_MakeAvailable(qflightinstruments)

        qt6_add_resources(QFLIGHTINSTRUMENTS_RESOURCE_SOURCES
            ${qflightinstruments_SOURCE_DIR}/qfi/qfi.qrc
        )

        add_library(qflightinstruments STATIC
            ${qflightinstruments_SOURCE_DIR}/qfi/qfi_Colors.cpp
            ${qflightinstruments_SOURCE_DIR}/qfi/qfi_Colors.h
            ${qflightinstruments_SOURCE_DIR}/qfi/qfi_EADI.cpp
            ${qflightinstruments_SOURCE_DIR}/qfi/qfi_EADI.h
            ${qflightinstruments_SOURCE_DIR}/qfi/qfi_Fonts.cpp
            ${qflightinstruments_SOURCE_DIR}/qfi/qfi_Fonts.h
            ${qflightinstruments_SOURCE_DIR}/qfi/qfi_defs.h
            ${QFLIGHTINSTRUMENTS_RESOURCE_SOURCES}
        )
        target_include_directories(qflightinstruments PUBLIC
            ${qflightinstruments_SOURCE_DIR}
        )
        target_link_libraries(qflightinstruments PUBLIC
            Qt6::Widgets
            Qt6::Svg
            Qt6::SvgWidgets
        )
        set_target_properties(qflightinstruments PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
        )
        set(QTTEST_HAS_QFLIGHTINSTRUMENTS TRUE)
    else()
        message(WARNING "QFlightInstruments disabled: Qt6 Svg/SvgWidgets not available")
    endif()
endif()

option(
    QTTEST_ENABLE_MIXR_SENSOR_PROVIDER
    "Build the optional MIXR radar sensor provider on macOS"
    ON
)
set(QTTEST_MIXR_GIT_TAG
    "bffebcec9b4c566f1394e8a6b34b7892a8f551c6"
    CACHE STRING "Pinned MIXR revision"
)

if(QTTEST_ENABLE_MIXR_SENSOR_PROVIDER AND APPLE)
    include(ExternalProject)
    find_package(Git REQUIRED)
    find_package(Threads REQUIRED)
    find_program(QTTEST_MAKE_EXECUTABLE NAMES gmake make REQUIRED)

    set(QTTEST_MIXR_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/mixr-src)
    set(QTTEST_MIXR_LIBRARY_DIR ${QTTEST_MIXR_SOURCE_DIR}/lib)
    set(QTTEST_MIXR_PLUGIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/sensor-plugins)
    set(QTTEST_MIXR_PLUGIN_PATH
        ${QTTEST_MIXR_PLUGIN_DIR}/libqttest_mixr_sensor.dylib)

    ExternalProject_Add(qttest_mixr_external
        EXCLUDE_FROM_ALL TRUE
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/mixr-build
        SOURCE_DIR ${QTTEST_MIXR_SOURCE_DIR}
        GIT_REPOSITORY https://github.com/doughodson/mixr.git
        GIT_TAG ${QTTEST_MIXR_GIT_TAG}
        GIT_SHALLOW FALSE
        UPDATE_DISCONNECTED TRUE
        PATCH_COMMAND
            ${CMAKE_COMMAND}
            -DMIXR_SOURCE_DIR=<SOURCE_DIR>
            -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ApplyMixrPatch.cmake
        CONFIGURE_COMMAND ""
        BUILD_COMMAND
            ${CMAKE_COMMAND} -E env
            MIXR_ROOT=<SOURCE_DIR>
            MIXR_3RD_PARTY_ROOT=<SOURCE_DIR>/thirdparty
            ${QTTEST_MAKE_EXECUTABLE} -C <SOURCE_DIR>/src/base -j2
        COMMAND
            ${CMAKE_COMMAND} -E env
            MIXR_ROOT=<SOURCE_DIR>
            MIXR_3RD_PARTY_ROOT=<SOURCE_DIR>/thirdparty
            ${QTTEST_MAKE_EXECUTABLE} -C <SOURCE_DIR>/src/simulation -j2
        COMMAND
            ${CMAKE_COMMAND} -E env
            MIXR_ROOT=<SOURCE_DIR>
            MIXR_3RD_PARTY_ROOT=<SOURCE_DIR>/thirdparty
            ${QTTEST_MAKE_EXECUTABLE} -C <SOURCE_DIR>/src/models -j2
        INSTALL_COMMAND ""
        BUILD_BYPRODUCTS
            ${QTTEST_MIXR_LIBRARY_DIR}/libmixr_base.a
            ${QTTEST_MIXR_LIBRARY_DIR}/libmixr_simulation.a
            ${QTTEST_MIXR_LIBRARY_DIR}/libmixr_models.a
    )

    add_library(qttest_mixr_sensor SHARED EXCLUDE_FROM_ALL
        integrations/mixr/MixrSensorPlugin.cpp
        src/infrastructure/sensors/SensorModelPluginApi.h
    )
    add_dependencies(qttest_mixr_sensor qttest_mixr_external)
    target_include_directories(qttest_mixr_sensor PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${QTTEST_MIXR_SOURCE_DIR}/include
    )
    target_link_libraries(qttest_mixr_sensor PRIVATE
        ${QTTEST_MIXR_LIBRARY_DIR}/libmixr_models.a
        ${QTTEST_MIXR_LIBRARY_DIR}/libmixr_simulation.a
        ${QTTEST_MIXR_LIBRARY_DIR}/libmixr_base.a
        Threads::Threads
    )
    set_target_properties(qttest_mixr_sensor PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${QTTEST_MIXR_PLUGIN_DIR}
        OUTPUT_NAME qttest_mixr_sensor
    )
    add_custom_target(qttest_setup_mixr DEPENDS qttest_mixr_sensor)
endif()

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
    src/application/CockpitControlService.cpp
    src/application/CockpitControlService.h
    src/application/SystemsTelemetry.cpp
    src/application/SystemsTelemetry.h
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
    src/infrastructure/ModelOrientation.cpp
    src/infrastructure/ModelOrientation.h
    src/infrastructure/SensorModelProviderCatalog.cpp
    src/infrastructure/SensorModelProviderCatalog.h
    src/infrastructure/SensorModelProviderBootstrap.cpp
    src/infrastructure/SensorModelProviderBootstrap.h
    src/infrastructure/sensors/SensorModelPluginApi.h
    src/infrastructure/sensors/SharedLibrarySensorModel.cpp
    src/infrastructure/sensors/SharedLibrarySensorModel.h
    src/infrastructure/sensors/StoneSoupSensorModel.cpp
    src/infrastructure/sensors/StoneSoupSensorModel.h
    src/infrastructure/Ros2TelemetryBackend.h
    ${QTTEST_ROS2_BACKEND_SOURCE}
    src/infrastructure/Ros2TelemetryContract.cpp
    src/infrastructure/Ros2TelemetryContract.h
    src/infrastructure/Ros2TelemetryPublisher.cpp
    src/infrastructure/Ros2TelemetryPublisher.h
    src/application/ScenarioState.cpp
    src/application/ScenarioState.h
    src/application/TacticalGraphicRepository.cpp
    src/application/TacticalGraphicRepository.h
    src/application/ScenarioSerializer.cpp
    src/application/ScenarioSerializer.h
    src/application/BehaviorEngine.cpp
    src/application/BehaviorEngine.h
    src/application/MunitionSimulator.cpp
    src/application/MunitionSimulator.h
    src/application/ScenarioQueries.cpp
    src/application/ScenarioQueries.h
    src/application/AttackTaskProcessor.cpp
    src/application/AttackTaskProcessor.h
    src/application/TaskApplicator.cpp
    src/application/TaskApplicator.h
    src/application/MovementIntent.cpp
    src/application/MovementIntent.h
    src/application/dynamics/IDynamicsModel.h
    src/application/dynamics/DynamicsBackendHealth.h
    src/application/dynamics/KinematicDynamicsModel.cpp
    src/application/dynamics/KinematicDynamicsModel.h
    src/application/KinematicsTelemetry.cpp
    src/application/KinematicsTelemetry.h
    src/application/JsbsimSetpointController.cpp
    src/application/JsbsimSetpointController.h
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
    src/application/SensorDetectionModel.cpp
    src/application/SensorDetectionModel.h
    src/application/sensors/ISensorModel.h
    src/application/sensors/NativeSensorModel.cpp
    src/application/sensors/NativeSensorModel.h
    src/application/sensors/SensorModelRegistry.cpp
    src/application/sensors/SensorModelRegistry.h
    src/domain/BombReleaseGate.cpp
    src/domain/BombReleaseGate.h
    src/domain/CombatRules.cpp
    src/domain/CombatRules.h
    src/domain/Entity.h
    src/domain/EntityIdentity.h
    src/domain/GeoMath.cpp
    src/domain/GeoMath.h
    src/domain/Sensor.cpp
    src/domain/Sensor.h
    src/domain/Task.cpp
    src/domain/Task.h
    src/presentation/AssignTaskController.cpp
    src/presentation/AssignTaskController.h
    src/presentation/BombReleaseController.cpp
    src/presentation/BombReleaseController.h
    src/presentation/BombReleaseActionsController.cpp
    src/presentation/BombReleaseActionsController.h
    src/presentation/EntityDefaultsResolver.cpp
    src/presentation/EntityDefaultsResolver.h
    src/presentation/MapBridgeScripts.cpp
    src/presentation/MapBridgeScripts.h
    src/presentation/EntityStateActionsController.cpp
    src/presentation/EntityStateActionsController.h
    src/presentation/ScenarioObjectEditorController.cpp
    src/presentation/ScenarioObjectEditorController.h
    src/presentation/SimulationLifecycleController.cpp
    src/presentation/SimulationLifecycleController.h
    src/presentation/TacticalGraphicsEditorController.cpp
    src/presentation/TacticalGraphicsEditorController.h
    src/presentation/TaskAssignmentController.cpp
    src/presentation/TaskAssignmentController.h
    src/presentation/WeaponActionsController.cpp
    src/presentation/WeaponActionsController.h
    src/presentation/GraphicPickCoordinator.cpp
    src/presentation/GraphicPickCoordinator.h
    src/presentation/PlanStepConfigurator.cpp
    src/presentation/PlanStepConfigurator.h
    src/presentation/Ros2TelemetryDialog.cpp
    src/presentation/Ros2TelemetryDialog.h
    src/presentation/DetectedContactsPresenter.cpp
    src/presentation/DetectedContactsPresenter.h
    src/presentation/KinematicsCockpitModel.cpp
    src/presentation/KinematicsCockpitModel.h
    src/presentation/KinematicsCockpitWidget.cpp
    src/presentation/KinematicsCockpitWidget.h
    src/presentation/EntityContextMenuBuilder.cpp
    src/presentation/EntityContextMenuBuilder.h
    src/presentation/EntityContextMenuStateBuilder.cpp
    src/presentation/EntityContextMenuStateBuilder.h
    src/presentation/TrackSetSyncer.h
    src/presentation/BombTargetMapSync.cpp
    src/presentation/BombTargetMapSync.h
    src/presentation/EntityPlanDialog.cpp
    src/presentation/EntityPlanDialog.h
    src/presentation/EntityHomePositionTracker.cpp
    src/presentation/EntityHomePositionTracker.h
    src/presentation/EntityPlanExecutor.cpp
    src/presentation/EntityPlanExecutor.h
    src/presentation/EntityStatusFormatter.cpp
    src/presentation/EntityStatusFormatter.h
    src/presentation/EntityTextFormatter.cpp
    src/presentation/EntityTextFormatter.h
    src/presentation/EntityVisualStateManager.cpp
    src/presentation/EntityVisualStateManager.h
    src/presentation/PlanTypes.h
    src/presentation/TrackIconProvider.cpp
    src/presentation/TrackIconProvider.h
    src/presentation/TrackSummaryBuilder.cpp
    src/presentation/TrackSummaryBuilder.h
)

if(QTTEST_HAS_MODERN_PFD)
    target_sources(qttest PRIVATE ${KINEMATICS_PFD_RESOURCE})
    target_link_libraries(qttest PRIVATE Qt6::Quick Qt6::QuickWidgets Qt6::Qml)
    target_compile_definitions(qttest PRIVATE QTTEST_HAS_MODERN_PFD=1)
endif()

target_link_libraries(qttest
    PRIVATE
        Qt6::Widgets
        Qt6::Gui
)

if(QTTEST_ENABLE_ROS2_TELEMETRY)
    target_link_libraries(qttest PRIVATE
        rclcpp::rclcpp
        qttest_interfaces::qttest_interfaces__rosidl_typesupport_cpp
    )
    if(APPLE)
        target_link_libraries(qttest PRIVATE Python3::Python)
    endif()
    target_compile_definitions(qttest PRIVATE QTTEST_HAS_ROS2_TELEMETRY=1)
endif()

if(QTTEST_HAS_QFLIGHTINSTRUMENTS)
    target_link_libraries(qttest PRIVATE qflightinstruments)
    target_compile_definitions(qttest PRIVATE QTTEST_HAS_QFLIGHTINSTRUMENTS=1)
endif()

if(TARGET libJSBSim)
    target_sources(qttest PRIVATE
        src/application/dynamics/JSBSimDynamicsModel.cpp
        src/application/dynamics/JSBSimDynamicsModel.h
    )
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
    QTTEST_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}"
)

find_package(Python3 QUIET COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
    if(WIN32)
        set(QTTEST_STONESOUP_VENV_PYTHON
            ${CMAKE_CURRENT_BINARY_DIR}/stonesoup-venv/Scripts/python.exe)
    else()
        set(QTTEST_STONESOUP_VENV_PYTHON
            ${CMAKE_CURRENT_BINARY_DIR}/stonesoup-venv/bin/python)
    endif()
    set(QTTEST_STONESOUP_READY_STAMP
        ${CMAKE_CURRENT_BINARY_DIR}/stonesoup-venv/.qttest-stonesoup-ready)
    add_custom_command(
        OUTPUT ${QTTEST_STONESOUP_READY_STAMP}
        COMMAND ${Python3_EXECUTABLE} -m venv
            ${CMAKE_CURRENT_BINARY_DIR}/stonesoup-venv
        COMMAND ${QTTEST_STONESOUP_VENV_PYTHON} -m pip
            install -r ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/stonesoup/requirements.txt
        COMMAND ${CMAKE_COMMAND} -E touch ${QTTEST_STONESOUP_READY_STAMP}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/stonesoup/requirements.txt
        COMMENT "Creating the isolated Stone Soup sensor provider environment"
        VERBATIM
    )
    add_custom_target(qttest_setup_stonesoup
        DEPENDS ${QTTEST_STONESOUP_READY_STAMP}
    )
endif()

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

option(QTTEST_BUILD_BENCHMARKS "Enable qttest benchmark targets" ON)
if(QTTEST_BUILD_BENCHMARKS AND TARGET libJSBSim)
    add_subdirectory(benchmarks)
endif()
