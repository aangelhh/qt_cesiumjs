# ---------------------------------------------------------------------------
# qttest unit tests
# ---------------------------------------------------------------------------
#
# GoogleTest is fetched via FetchContent so this works out of the box without
# any system-wide install. Tests link against Qt6::Core directly (rather than
# against the main qttest target) because the domain code still relies on
# QString/QVector — once domain/ is decoupled from Qt we can drop the Qt dep
# here.

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)

# Don't override the parent project's compiler/linker settings on Windows.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(qttest_unit_tests
    domain/TaskTest.cpp
    domain/GeoMathTest.cpp
    domain/BombReleaseGateTest.cpp
    domain/CombatRulesTest.cpp
    domain/SensorTest.cpp
    application/EventBusTest.cpp
    application/AttackTaskProcessorTest.cpp
    application/TaskApplicatorTest.cpp
    application/FlightDynamicsEngineConsistencyTest.cpp
    application/DynamicsBackendHealthTest.cpp
    application/RuntimeBenchmarkStatisticsTest.cpp
    application/KinematicDynamicsModelTest.cpp
    application/MovementIntentTest.cpp
    application/KinematicsTelemetryTest.cpp
    application/JsbsimSetpointControllerTest.cpp
    application/CockpitControlServiceTest.cpp
    application/SystemsTelemetryTest.cpp
    infrastructure/ModelCatalogTest.cpp
    infrastructure/ModelOrientationTest.cpp
    application/MunitionSimulatorTest.cpp
    application/BehaviorEngineTest.cpp
    application/SensorEngineTest.cpp
    application/ScenarioQueriesTest.cpp
    application/ScenarioSerializerTest.cpp
    application/TacticalGraphicRepositoryTest.cpp
    infrastructure/CesiumScenePageTest.cpp
    presentation/EntityVisualStateManagerTest.cpp
    presentation/EntityHomePositionTrackerTest.cpp
    presentation/TrackIconProviderTest.cpp
    presentation/PlanStepConfiguratorTest.cpp
    presentation/EntityPlanExecutorConditionalTest.cpp
    presentation/GraphicPickCoordinatorTest.cpp
    presentation/EntityStatusFormatterTest.cpp
    presentation/EntityTrackSummaryTest.cpp
    presentation/DetectedContactsPresenterTest.cpp
    presentation/KinematicsCockpitModelTest.cpp
    presentation/WeaponActionsControllerTest.cpp
    presentation/EntityStateActionsControllerTest.cpp
    presentation/TaskAssignmentControllerTest.cpp
    presentation/TacticalGraphicsEditorControllerTest.cpp
    presentation/BombReleaseActionsControllerTest.cpp
    presentation/ScenarioObjectEditorControllerTest.cpp
    presentation/SimulationLifecycleControllerTest.cpp
    presentation/AssignTaskControllerTest.cpp
    presentation/EntityDefaultsResolverTest.cpp
    presentation/MapBridgeScriptsTest.cpp
    presentation/EntityContextMenuStateBuilderTest.cpp
    presentation/BombTargetMapSyncTest.cpp
    # Sources under test. We re-compile them here rather than link against the
    # qttest executable target (which is a Qt application bundle). When the
    # core code is moved into a static library this list will collapse into a
    # single target_link_libraries() call.
    ${CMAKE_SOURCE_DIR}/src/domain/Task.cpp
    ${CMAKE_SOURCE_DIR}/src/domain/GeoMath.cpp
    ${CMAKE_SOURCE_DIR}/src/domain/BombReleaseGate.cpp
    ${CMAKE_SOURCE_DIR}/src/domain/CombatRules.cpp
    ${CMAKE_SOURCE_DIR}/src/domain/Sensor.cpp
    ${CMAKE_SOURCE_DIR}/src/application/ScenarioState.cpp
    ${CMAKE_SOURCE_DIR}/src/application/TacticalGraphicRepository.cpp
    ${CMAKE_SOURCE_DIR}/src/application/ScenarioSerializer.cpp
    ${CMAKE_SOURCE_DIR}/src/application/BehaviorEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/application/MunitionSimulator.cpp
    ${CMAKE_SOURCE_DIR}/src/application/ScenarioQueries.cpp
    ${CMAKE_SOURCE_DIR}/src/application/AttackTaskProcessor.cpp
    ${CMAKE_SOURCE_DIR}/src/application/TaskApplicator.cpp
    ${CMAKE_SOURCE_DIR}/src/application/MovementIntent.cpp
    ${CMAKE_SOURCE_DIR}/src/application/dynamics/KinematicDynamicsModel.cpp
    ${CMAKE_SOURCE_DIR}/src/application/CockpitControlService.cpp
    ${CMAKE_SOURCE_DIR}/src/application/KinematicsTelemetry.cpp
    ${CMAKE_SOURCE_DIR}/src/application/JsbsimSetpointController.cpp
    ${CMAKE_SOURCE_DIR}/src/application/SystemsTelemetry.cpp
    ${CMAKE_SOURCE_DIR}/src/application/Command.cpp
    ${CMAKE_SOURCE_DIR}/src/infrastructure/CesiumScenePage.cpp
    ${CMAKE_SOURCE_DIR}/src/infrastructure/ModelCatalog.cpp
    ${CMAKE_SOURCE_DIR}/src/infrastructure/ModelOrientation.cpp
    ${CMAKE_SOURCE_DIR}/src/application/SimulationEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/application/SensorEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/application/FlightDynamicsEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/presentation/BombReleaseController.cpp
    ${CMAKE_SOURCE_DIR}/src/presentation/WeaponActionsController.cpp
    ${CMAKE_SOURCE_DIR}/src/presentation/EntityStateActionsController.cpp
    ${CMAKE_SOURCE_DIR}/src/presentation/TaskAssignmentController.cpp
    ${CMAKE_SOURCE_DIR}/src/presentation/TacticalGraphicsEditorController.cpp
    ${CMAKE_SOURCE_DIR}/src/presentation/BombReleaseActionsController.cpp
    ${CMAKE_SOURCE_DIR}/src/presentation/ScenarioObjectEditorController.cpp
    ${CMAKE_SOURCE_DIR}/src/presentation/SimulationLifecycleController.cpp
    ${CMAKE_SOURCE_DIR}/src/presentation/AssignTaskController.cpp
    ${CMAKE_SOURCE_DIR}/src/presentation/EntityDefaultsResolver.cpp
    ${CMAKE_SOURCE_DIR}/src/presentation/MapBridgeScripts.cpp
    ${CMAKE_SOURCE_DIR}/src/presentation/EntityContextMenuStateBuilder.cpp
    ${CMAKE_SOURCE_DIR}/src/presentation/BombTargetMapSync.cpp
    ${CMAKE_SOURCE_DIR}/src/presentation/EntityVisualStateManager.cpp
    ${CMAKE_SOURCE_DIR}/src/presentation/EntityHomePositionTracker.cpp
    ${CMAKE_SOURCE_DIR}/src/presentation/TrackIconProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/presentation/PlanStepConfigurator.cpp
    ${CMAKE_SOURCE_DIR}/src/presentation/GraphicPickCoordinator.cpp
    ${CMAKE_SOURCE_DIR}/src/presentation/TrackSummaryBuilder.cpp
    ${CMAKE_SOURCE_DIR}/src/presentation/EntityStatusFormatter.cpp
    ${CMAKE_SOURCE_DIR}/src/presentation/DetectedContactsPresenter.cpp
    ${CMAKE_SOURCE_DIR}/src/presentation/KinematicsCockpitModel.cpp
    ${CMAKE_SOURCE_DIR}/src/presentation/EntityPlanExecutor.cpp
)

target_include_directories(qttest_unit_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_compile_definitions(qttest_unit_tests PRIVATE
    QTTEST_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

target_link_libraries(qttest_unit_tests
    PRIVATE
        Qt6::Core
        Qt6::Gui
        GTest::gtest
        GTest::gtest_main
)

if(NOT MSVC)
    target_compile_options(qttest_unit_tests PRIVATE -Wall -Wextra -Wno-unused-parameter)
endif()

include(GoogleTest)
gtest_discover_tests(qttest_unit_tests)

if(TARGET qflightinstruments OR QTTEST_HAS_MODERN_PFD)
    add_executable(qttest_kinematics_cockpit_smoke
        presentation/KinematicsCockpitWidgetSmoke.cpp
        ${CMAKE_SOURCE_DIR}/src/presentation/KinematicsCockpitModel.cpp
        ${CMAKE_SOURCE_DIR}/src/presentation/KinematicsCockpitWidget.cpp
    )
    if(QTTEST_HAS_MODERN_PFD)
        target_sources(qttest_kinematics_cockpit_smoke PRIVATE
            ${KINEMATICS_PFD_RESOURCE}
        )
        target_link_libraries(qttest_kinematics_cockpit_smoke PRIVATE
            Qt6::Quick
            Qt6::QuickWidgets
            Qt6::Qml
        )
        target_compile_definitions(qttest_kinematics_cockpit_smoke PRIVATE
            QTTEST_HAS_MODERN_PFD=1
        )
    endif()
    target_include_directories(qttest_kinematics_cockpit_smoke PRIVATE
        ${CMAKE_SOURCE_DIR}/src
    )
    target_link_libraries(qttest_kinematics_cockpit_smoke PRIVATE Qt6::Widgets)
    if(TARGET qflightinstruments)
        target_link_libraries(qttest_kinematics_cockpit_smoke PRIVATE
            qflightinstruments
        )
        target_compile_definitions(qttest_kinematics_cockpit_smoke PRIVATE
            QTTEST_HAS_QFLIGHTINSTRUMENTS=1
        )
    endif()
    add_test(
        NAME KinematicsCockpitWidget.RendersInstrumentPanels
        COMMAND qttest_kinematics_cockpit_smoke
    )
    set_tests_properties(
        KinematicsCockpitWidget.RendersInstrumentPanels
        PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QUICK_BACKEND=software"
    )
endif()

if(TARGET libJSBSim)
    add_executable(qttest_jsbsim_control_smoke
        application/JsbsimFlightControlSmoke.cpp
        ${CMAKE_SOURCE_DIR}/src/application/FlightDynamicsEngine.cpp
        ${CMAKE_SOURCE_DIR}/src/application/dynamics/KinematicDynamicsModel.cpp
        ${CMAKE_SOURCE_DIR}/src/application/dynamics/JSBSimDynamicsModel.cpp
        ${CMAKE_SOURCE_DIR}/src/application/JsbsimSetpointController.cpp
        ${CMAKE_SOURCE_DIR}/src/application/MovementIntent.cpp
        ${CMAKE_SOURCE_DIR}/src/application/SystemsTelemetry.cpp
        ${CMAKE_SOURCE_DIR}/src/domain/GeoMath.cpp
        ${CMAKE_SOURCE_DIR}/src/domain/Task.cpp
    )
    target_include_directories(qttest_jsbsim_control_smoke PRIVATE
        ${CMAKE_SOURCE_DIR}/src
    )
    target_compile_definitions(qttest_jsbsim_control_smoke PRIVATE
        QTTEST_HAS_JSBSIM=1
        QTTEST_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
    )
    target_link_libraries(qttest_jsbsim_control_smoke PRIVATE
        Qt6::Core
        GTest::gtest
        GTest::gtest_main
        libJSBSim
    )
    add_test(
        NAME JsbsimFlightControl.FighterSetpointsRemainStable
        COMMAND qttest_jsbsim_control_smoke
    )

    add_executable(qttest_jsbsim_dynamics_model_test
        application/JSBSimDynamicsModelTest.cpp
        ${CMAKE_SOURCE_DIR}/src/application/dynamics/JSBSimDynamicsModel.cpp
        ${CMAKE_SOURCE_DIR}/src/application/JsbsimSetpointController.cpp
        ${CMAKE_SOURCE_DIR}/src/application/SystemsTelemetry.cpp
        ${CMAKE_SOURCE_DIR}/src/domain/GeoMath.cpp
        ${CMAKE_SOURCE_DIR}/src/domain/Task.cpp
    )
    target_include_directories(qttest_jsbsim_dynamics_model_test PRIVATE
        ${CMAKE_SOURCE_DIR}/src
    )
    target_compile_definitions(qttest_jsbsim_dynamics_model_test PRIVATE
        QTTEST_HAS_JSBSIM=1
        QTTEST_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
    )
    target_link_libraries(qttest_jsbsim_dynamics_model_test PRIVATE
        Qt6::Core
        GTest::gtest
        GTest::gtest_main
        libJSBSim
    )
    add_test(
        NAME JSBSimDynamicsModel.LifecycleAndStep
        COMMAND qttest_jsbsim_dynamics_model_test
    )
endif()
