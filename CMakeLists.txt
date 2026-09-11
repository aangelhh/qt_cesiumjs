cmake_minimum_required(VERSION 3.21)

project(qttest VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Xml Network)
find_package(Qt6 QUIET COMPONENTS WebEngineWidgets WebEngineCore WebChannel Positioning)

add_library(qttest_geodesic STATIC
    Dependencies/geographiclib-c/geodesic.c
)
set_target_properties(qttest_geodesic PROPERTIES AUTOMOC OFF)
target_include_directories(qttest_geodesic PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/geographiclib-c
)

add_library(qttest_geospatial STATIC
    src/geospatial/GeographicLibGeospatialService.cpp
    src/geospatial/GeographicLibGeospatialService.h
    src/geospatial/IGeospatialService.h
    src/domain/GeoMath.cpp
    src/domain/GeoMath.h
)
target_include_directories(qttest_geospatial PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(qttest_geospatial PUBLIC
    qttest_geodesic
    Qt6::Core
)

# ---------------------------------------------------------------------------
# DIS 7 runtime (IEEE 1278.1-2012)
# ---------------------------------------------------------------------------
option(QTTEST_ENABLE_DIS "Build the DIS 7 UDP gateway" ON)
if(QTTEST_ENABLE_DIS AND
   EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/KDIS/CMakeLists.txt")
    set(_QTTEST_CXX_STANDARD ${CMAKE_CXX_STANDARD})
    set(DIS_VERSION 7 CACHE STRING "KDIS protocol version" FORCE)
    set(KDIS_USE_ENUM_DESCRIPTORS OFF CACHE BOOL "" FORCE)
    set(KDIS_DEPRECATED_NET OFF CACHE BOOL "" FORCE)
    set(KDIS_BUILD_INSTALL OFF CACHE BOOL "" FORCE)
    set(KDIS_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(KDIS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(KDIS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    add_subdirectory(Dependencies/KDIS EXCLUDE_FROM_ALL)
    set(CMAKE_CXX_STANDARD ${_QTTEST_CXX_STANDARD})

    # KDIS selects the header layout with DIS_VERSION but keeps the definition
    # private; consumers of its public headers need the same value.
    target_compile_definitions(KDIS PUBLIC DIS_VERSION=7)

    add_library(qttest_dis_core STATIC
        src/infrastructure/interoperability/dis/DisSimulationControlCodec.cpp
        src/infrastructure/interoperability/dis/DisEmissionCodec.cpp
        src/infrastructure/interoperability/dis/DisInteractionCodec.cpp
        src/infrastructure/interoperability/dis/DisEntityStateCodec.cpp
        src/infrastructure/interoperability/dis/DisEntityStateCodec.h
        src/infrastructure/interoperability/dis/DisGateway.cpp
        src/infrastructure/interoperability/dis/DisGateway.h
        src/infrastructure/interoperability/dis/DisRemoteEntityRegistry.cpp
        src/infrastructure/interoperability/dis/DisRemoteEntityRegistry.h
        src/infrastructure/interoperability/dis/DisWarfareCodec.cpp
        src/infrastructure/interoperability/dis/DisWarfareCodec.h
        src/infrastructure/interoperability/dis/DisTypes.h
    )
    target_include_directories(qttest_dis_core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
    target_link_libraries(qttest_dis_core PUBLIC
        KDIS::KDIS
        qttest_geospatial
        Qt6::Core
        Qt6::Network
    )
    target_compile_definitions(qttest_dis_core PUBLIC QTTEST_HAS_DIS=1)
    add_executable(qttest_dis_pdu_tester Tools/dis/DisPduTester.cpp)
    target_link_libraries(qttest_dis_pdu_tester PRIVATE qttest_dis_core Qt6::Widgets)
endif()

# ---------------------------------------------------------------------------
# Open-source HLA runtime abstraction
# ---------------------------------------------------------------------------
add_library(tactical_hla_core STATIC
    src/infrastructure/interoperability/hla/HlaEntityPublisher.cpp
    src/infrastructure/interoperability/hla/HlaEntityPublisher.h
    src/infrastructure/interoperability/hla/HlaRuntime.cpp
    src/infrastructure/interoperability/hla/HlaRuntime.h
    src/infrastructure/interoperability/hla/HlaWarfarePublisher.cpp
    src/infrastructure/interoperability/hla/HlaWarfarePublisher.h
    src/infrastructure/interoperability/hla/HlaInboundAdapter.cpp
    src/infrastructure/interoperability/hla/HlaInboundAdapter.h
    src/infrastructure/interoperability/hla/HlaSimulationControlPublisher.cpp
    src/infrastructure/interoperability/hla/HlaSimulationControlPublisher.h
    src/infrastructure/interoperability/hla/HlaSensorPublisher.cpp
    src/infrastructure/interoperability/hla/HlaSensorPublisher.h
    src/infrastructure/interoperability/hla/HlaTypes.h
    src/infrastructure/interoperability/hla/IHlaBackend.h
    src/infrastructure/interoperability/hla/MockHlaBackend.cpp
    src/infrastructure/interoperability/hla/MockHlaBackend.h
    src/infrastructure/interoperability/hla/RprFomEncoding.cpp
    src/infrastructure/interoperability/hla/RprFomEncoding.h
)
target_include_directories(tactical_hla_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(tactical_hla_core PUBLIC qttest_geospatial)

add_library(qttest_hla_infrastructure STATIC
    src/infrastructure/interoperability/hla/HlaBackendFactory.cpp
    src/infrastructure/interoperability/hla/HlaBackendFactory.h
    src/infrastructure/interoperability/hla/HlaBackendPluginApi.h
    src/infrastructure/interoperability/hla/OpenRtiCompatibilityBackend.cpp
    src/infrastructure/interoperability/hla/OpenRtiCompatibilityBackend.h
    src/infrastructure/interoperability/hla/SharedLibraryHlaBackend.cpp
    src/infrastructure/interoperability/hla/SharedLibraryHlaBackend.h
)
target_include_directories(qttest_hla_infrastructure PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(qttest_hla_infrastructure PUBLIC
    tactical_hla_core
    Qt6::Core
    Qt6::Xml
)

set(QTTEST_HLA_PLUGIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/hla-plugins)

add_library(qttest_hla_mock_backend SHARED
    integrations/hla/mock/MockHlaBackendPlugin.cpp
    src/infrastructure/interoperability/hla/HlaBackendPluginApi.h
)
target_include_directories(qttest_hla_mock_backend PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
set_target_properties(qttest_hla_mock_backend PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${QTTEST_HLA_PLUGIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${QTTEST_HLA_PLUGIN_DIR}
    OUTPUT_NAME qttest_hla_mock_backend
)
set(QTTEST_HLA_MOCK_PLUGIN_PATH
    ${QTTEST_HLA_PLUGIN_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}qttest_hla_mock_backend${CMAKE_SHARED_LIBRARY_SUFFIX})

option(
    QTTEST_ENABLE_OPENRTI_HLA_BACKEND
    "Build the open-source OpenRTI IEEE 1516e backend"
    ON
)
set(QTTEST_OPENRTI_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/OpenRTI
    CACHE PATH "OpenRTI source tree"
)
set(QTTEST_OPENRTI_ROOT
    "$ENV{QTTEST_OPENRTI_ROOT}"
    CACHE PATH "Prebuilt OpenRTI installation"
)

set(QTTEST_OPENRTI_AVAILABLE OFF)
set(QTTEST_OPENRTI_EXTERNAL_TARGET "")
if(QTTEST_ENABLE_OPENRTI_HLA_BACKEND AND QTTEST_OPENRTI_ROOT)
    set(QTTEST_OPENRTI_INSTALL_DIR ${QTTEST_OPENRTI_ROOT})
    set(QTTEST_OPENRTI_AVAILABLE ON)
    set(QTTEST_OPENRTI_ORIGIN "prebuilt installation ${QTTEST_OPENRTI_ROOT}")
elseif(QTTEST_ENABLE_OPENRTI_HLA_BACKEND AND
       EXISTS "${QTTEST_OPENRTI_SOURCE_DIR}/CMakeLists.txt")
    include(ExternalProject)
    set(QTTEST_OPENRTI_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/openrti-install)
    set(QTTEST_OPENRTI_LIBRARY_DIR ${QTTEST_OPENRTI_INSTALL_DIR}/lib)
    if(WIN32)
        set(QTTEST_OPENRTI_RTI1516E_LIBRARY
            ${QTTEST_OPENRTI_LIBRARY_DIR}/rti1516e.lib)
        set(QTTEST_OPENRTI_FEDTIME1516E_LIBRARY
            ${QTTEST_OPENRTI_LIBRARY_DIR}/fedtime1516e.lib)
    else()
        set(QTTEST_OPENRTI_RTI1516E_LIBRARY
            ${QTTEST_OPENRTI_LIBRARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}rti1516e${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(QTTEST_OPENRTI_FEDTIME1516E_LIBRARY
            ${QTTEST_OPENRTI_LIBRARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}fedtime1516e${CMAKE_SHARED_LIBRARY_SUFFIX})
    endif()
    file(MAKE_DIRECTORY ${QTTEST_OPENRTI_INSTALL_DIR}/include/rti1516e)

    ExternalProject_Add(qttest_openrti_external
        SOURCE_DIR ${QTTEST_OPENRTI_SOURCE_DIR}
        BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/openrti-build
        INSTALL_DIR ${QTTEST_OPENRTI_INSTALL_DIR}
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_CXX_STANDARD=11
            -DCMAKE_INSTALL_LIBDIR=lib
            -DCMAKE_INSTALL_INCLUDEDIR=include
            -DOPENRTI_BUILD_SHARED=ON
            -DOPENRTI_ENABLE_RTI13=OFF
            -DOPENRTI_ENABLE_RTI1516=OFF
            -DOPENRTI_ENABLE_RTI1516E=ON
            -DOPENRTI_ENABLE_PYTHON_BINDINGS=OFF
            -DOPENRTI_INSTALL_WITH_RPATH=OFF
        BUILD_BYPRODUCTS
            ${QTTEST_OPENRTI_RTI1516E_LIBRARY}
            ${QTTEST_OPENRTI_FEDTIME1516E_LIBRARY}
        TEST_COMMAND ""
        UPDATE_COMMAND ""
    )
    set(QTTEST_OPENRTI_AVAILABLE ON)
    set(QTTEST_OPENRTI_EXTERNAL_TARGET qttest_openrti_external)
    set(QTTEST_OPENRTI_ORIGIN "source tree ${QTTEST_OPENRTI_SOURCE_DIR}")
endif()

if(QTTEST_OPENRTI_AVAILABLE)
    set(QTTEST_OPENRTI_LIBRARY_DIR ${QTTEST_OPENRTI_INSTALL_DIR}/lib)
    if(WIN32)
        set(QTTEST_OPENRTI_RTI1516E_LIBRARY
            ${QTTEST_OPENRTI_LIBRARY_DIR}/rti1516e.lib)
        set(QTTEST_OPENRTI_FEDTIME1516E_LIBRARY
            ${QTTEST_OPENRTI_LIBRARY_DIR}/fedtime1516e.lib)
    else()
        set(QTTEST_OPENRTI_RTI1516E_LIBRARY
            ${QTTEST_OPENRTI_LIBRARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}rti1516e${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(QTTEST_OPENRTI_FEDTIME1516E_LIBRARY
            ${QTTEST_OPENRTI_LIBRARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}fedtime1516e${CMAKE_SHARED_LIBRARY_SUFFIX})
    endif()

    if(QTTEST_OPENRTI_ROOT)
        if(NOT EXISTS "${QTTEST_OPENRTI_INSTALL_DIR}/include/rti1516e/RTI/RTI1516.h" OR
           NOT EXISTS "${QTTEST_OPENRTI_RTI1516E_LIBRARY}" OR
           NOT EXISTS "${QTTEST_OPENRTI_FEDTIME1516E_LIBRARY}")
            message(FATAL_ERROR
                "QTTEST_OPENRTI_ROOT does not contain the required OpenRTI headers and libraries: "
                "${QTTEST_OPENRTI_ROOT}")
        endif()
    endif()

    add_library(qttest_hla_openrti1516e_backend SHARED
        integrations/hla/pitch1516e/Pitch1516eBackendPlugin.cpp
        src/infrastructure/interoperability/hla/HlaBackendPluginApi.h
    )
    if(QTTEST_OPENRTI_EXTERNAL_TARGET)
        add_dependencies(qttest_hla_openrti1516e_backend ${QTTEST_OPENRTI_EXTERNAL_TARGET})
    endif()
    target_include_directories(qttest_hla_openrti1516e_backend PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${QTTEST_OPENRTI_INSTALL_DIR}/include/rti1516e
    )
    target_compile_definitions(qttest_hla_openrti1516e_backend PRIVATE
        QTTEST_HLA_OPENRTI_BACKEND=1
        QTTEST_HLA_1516E_BACKEND_ID="openrti1516e"
        QTTEST_HLA_1516E_BACKEND_NAME="OpenRTI IEEE 1516e"
    )
    target_link_libraries(qttest_hla_openrti1516e_backend PRIVATE
        ${QTTEST_OPENRTI_RTI1516E_LIBRARY}
        ${QTTEST_OPENRTI_FEDTIME1516E_LIBRARY}
    )
    if(APPLE)
        target_link_options(qttest_hla_openrti1516e_backend PRIVATE
            "LINKER:-needed_library,${QTTEST_OPENRTI_FEDTIME1516E_LIBRARY}"
        )
    endif()
    set_target_properties(qttest_hla_openrti1516e_backend PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED ON
        LIBRARY_OUTPUT_DIRECTORY ${QTTEST_HLA_PLUGIN_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${QTTEST_HLA_PLUGIN_DIR}
        OUTPUT_NAME qttest_hla_openrti1516e_backend
        BUILD_RPATH ${QTTEST_OPENRTI_LIBRARY_DIR}
    )
    set(QTTEST_HLA_OPENRTI_PLUGIN_PATH
        ${QTTEST_HLA_PLUGIN_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}qttest_hla_openrti1516e_backend${CMAKE_SHARED_LIBRARY_SUFFIX})
    message(STATUS "OpenRTI IEEE 1516e backend enabled from ${QTTEST_OPENRTI_ORIGIN}")
else()
    message(STATUS "OpenRTI IEEE 1516e backend disabled: installation or source tree not found")
endif()

option(
    QTTEST_ENABLE_PITCH_HLA_BACKEND
    "Build the optional Pitch pRTI IEEE 1516e backend when locally installed"
    ON
)
set(
    QTTEST_PITCH_RTI_ROOT
    "$ENV{PitchRTI_ROOT}"
    CACHE PATH "Local Pitch pRTI installation used only to build the optional backend"
)
if(NOT QTTEST_PITCH_RTI_ROOT AND APPLE AND EXISTS "/Applications/prti1516e")
    set(QTTEST_PITCH_RTI_ROOT "/Applications/prti1516e")
endif()

if(QTTEST_ENABLE_PITCH_HLA_BACKEND AND QTTEST_PITCH_RTI_ROOT)
    find_package(PitchRTI QUIET CONFIG
        PATHS ${QTTEST_PITCH_RTI_ROOT}/cmake
        NO_DEFAULT_PATH
    )
endif()

if(QTTEST_ENABLE_PITCH_HLA_BACKEND AND TARGET PitchRTI::librti1516e)
    add_library(qttest_hla_pitch1516e_backend SHARED
        integrations/hla/pitch1516e/Pitch1516eBackendPlugin.cpp
        src/infrastructure/interoperability/hla/HlaBackendPluginApi.h
    )
    target_include_directories(qttest_hla_pitch1516e_backend PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
    target_compile_definitions(qttest_hla_pitch1516e_backend PRIVATE
        QTTEST_HLA_1516E_BACKEND_ID="pitch1516e"
        QTTEST_HLA_1516E_BACKEND_NAME="pRTI IEEE 1516e"
    )
    target_link_libraries(qttest_hla_pitch1516e_backend PRIVATE
        PitchRTI::librti1516e
    )
    set_target_properties(qttest_hla_pitch1516e_backend PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
        LIBRARY_OUTPUT_DIRECTORY ${QTTEST_HLA_PLUGIN_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${QTTEST_HLA_PLUGIN_DIR}
        OUTPUT_NAME qttest_hla_pitch1516e_backend
    )
    get_target_property(
        QTTEST_PITCH_RTI_LIBRARY
        PitchRTI::librti1516e
        IMPORTED_LOCATION
    )
    get_filename_component(
        QTTEST_PITCH_RTI_LIBRARY_DIR
        ${QTTEST_PITCH_RTI_LIBRARY}
        DIRECTORY
    )
    set_target_properties(qttest_hla_pitch1516e_backend PROPERTIES
        BUILD_RPATH
            "${QTTEST_PITCH_RTI_LIBRARY_DIR};${QTTEST_PITCH_RTI_ROOT}/lib;${QTTEST_PITCH_RTI_ROOT}/jre/lib/server"
    )
    set(QTTEST_HLA_PITCH_PLUGIN_PATH
        ${QTTEST_HLA_PLUGIN_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}qttest_hla_pitch1516e_backend${CMAKE_SHARED_LIBRARY_SUFFIX})
    message(STATUS "Pitch IEEE 1516e backend enabled from ${QTTEST_PITCH_RTI_ROOT}")
else()
    message(STATUS "Pitch IEEE 1516e backend disabled: local pRTI SDK not found")
endif()

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
    "Build the MIXR radar sensor provider"
    ON
)
set(QTTEST_MIXR_GIT_TAG
    "bffebcec9b4c566f1394e8a6b34b7892a8f551c6"
    CACHE STRING "Pinned MIXR revision"
)

if(QTTEST_ENABLE_MIXR_SENSOR_PROVIDER)
    include(cmake/MixrSensorProvider.cmake)
endif()

qt_standard_project_setup()

option(QTTEST_ENABLE_JSBSIM "Enable JSBSim flight dynamics backend when available" ON)
set(QTTEST_JSBSIM_ROOT
    "$ENV{QTTEST_JSBSIM_ROOT}"
    CACHE PATH "Prebuilt JSBSim installation"
)

if(QTTEST_ENABLE_JSBSIM AND QTTEST_JSBSIM_ROOT)
    if(WIN32)
        set(QTTEST_JSBSIM_LIBRARY "${QTTEST_JSBSIM_ROOT}/lib/JSBSim.lib")
    else()
        set(QTTEST_JSBSIM_LIBRARY "${QTTEST_JSBSIM_ROOT}/lib/libJSBSim.a")
    endif()
    if(NOT EXISTS "${QTTEST_JSBSIM_ROOT}/include/JSBSim/FGFDMExec.h" OR
       NOT EXISTS "${QTTEST_JSBSIM_LIBRARY}")
        message(FATAL_ERROR
            "QTTEST_JSBSIM_ROOT does not contain the required JSBSim headers and library: "
            "${QTTEST_JSBSIM_ROOT}")
    endif()

    add_library(libJSBSim STATIC IMPORTED GLOBAL)
    set_target_properties(libJSBSim PROPERTIES
        IMPORTED_LOCATION "${QTTEST_JSBSIM_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${QTTEST_JSBSIM_ROOT}/include/JSBSim"
    )
    if(WIN32)
        target_compile_definitions(libJSBSim INTERFACE JSBSIM_STATIC_LINK _USE_MATH_DEFINES)
        target_link_libraries(libJSBSim INTERFACE wsock32 ws2_32)
    elseif(UNIX)
        target_link_libraries(libJSBSim INTERFACE m)
    endif()
    message(STATUS "JSBSim enabled from prebuilt installation ${QTTEST_JSBSIM_ROOT}")
elseif(QTTEST_ENABLE_JSBSIM AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/jsbsim/CMakeLists.txt")
    set(BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(BUILD_PYTHON_MODULE OFF CACHE BOOL "" FORCE)
    set(BUILD_JULIA_PACKAGE OFF CACHE BOOL "" FORCE)
    set(BUILD_MATLAB_SFUNCTION OFF CACHE BOOL "" FORCE)
    add_subdirectory(Dependencies/jsbsim EXCLUDE_FROM_ALL)
    message(STATUS "JSBSim enabled from source tree ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/jsbsim")
else()
    message(STATUS "JSBSim disabled: installation or source tree not found")
endif()

qt_add_executable(qttest
    main.cpp
    src/application/HlaCombatDemoScenario.cpp
    src/application/HlaCombatDemoScenario.h
    src/application/HlaStartupSession.cpp
    src/application/HlaStartupSession.h
    src/application/StartupConfiguration.cpp
    src/application/StartupConfiguration.h
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
    src/infrastructure/JsbsimAircraftCatalog.cpp
    src/infrastructure/JsbsimAircraftCatalog.h
    src/infrastructure/JsbsimModelRepository.cpp
    src/infrastructure/JsbsimModelRepository.h
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
    src/presentation/HlaConnectionPanel.cpp
    src/presentation/HlaConnectionPanel.h
    src/presentation/PlanStepConfigurator.cpp
    src/presentation/PlanStepConfigurator.h
    src/presentation/Ros2TelemetryDialog.cpp
    src/presentation/Ros2TelemetryDialog.h
    src/presentation/StartupConfigurationDialog.cpp
    src/presentation/StartupConfigurationDialog.h
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

qt_add_resources(qttest "windows_theme"
    PREFIX "/"
    FILES Data/windows-modern.qss
)

if(TARGET qttest_mixr_sensor)
    add_dependencies(qttest qttest_mixr_sensor)
endif()

if(QTTEST_HAS_MODERN_PFD)
    target_sources(qttest PRIVATE ${KINEMATICS_PFD_RESOURCE})
    target_link_libraries(qttest PRIVATE Qt6::Quick Qt6::QuickWidgets Qt6::Qml)
    target_compile_definitions(qttest PRIVATE QTTEST_HAS_MODERN_PFD=1)
endif()

target_link_libraries(qttest
    PRIVATE
        qttest_geospatial
        qttest_hla_infrastructure
        Qt6::Widgets
        Qt6::Gui
)

if(TARGET qttest_dis_core)
    target_sources(qttest PRIVATE
        src/application/DisStartupSession.cpp
        src/application/DisStartupSession.h
        src/presentation/DisConnectionPanel.cpp
        src/presentation/DisConnectionPanel.h
    )
    target_link_libraries(qttest PRIVATE qttest_dis_core)
    add_dependencies(qttest qttest_dis_pdu_tester)
    target_compile_definitions(qttest PRIVATE QTTEST_DIS_TESTER_PATH="$<TARGET_FILE:qttest_dis_pdu_tester>")
endif()

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

if(DEFINED QTTEST_HLA_OPENRTI_PLUGIN_PATH)
    target_compile_definitions(qttest PRIVATE
        QTTEST_HLA_OPENRTI_PLUGIN_PATH="${QTTEST_HLA_OPENRTI_PLUGIN_PATH}"
    )
    add_dependencies(qttest qttest_hla_openrti1516e_backend)
endif()
if(DEFINED QTTEST_HLA_PITCH_PLUGIN_PATH)
    target_compile_definitions(qttest PRIVATE
        QTTEST_HLA_PITCH_PLUGIN_PATH="${QTTEST_HLA_PITCH_PLUGIN_PATH}"
    )
    add_dependencies(qttest qttest_hla_pitch1516e_backend)
endif()

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
