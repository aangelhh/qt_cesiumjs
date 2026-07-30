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
    application/MovementIntentTest.cpp
    application/KinematicsTelemetryTest.cpp
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
    ${CMAKE_SOURCE_DIR}/src/application/KinematicsTelemetry.cpp
    ${CMAKE_SOURCE_DIR}/src/application/Command.cpp
    ${CMAKE_SOURCE_DIR}/src/infrastructure/CesiumScenePage.cpp
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
    ${CMAKE_SOURCE_DIR}/src/presentation/EntityPlanExecutor.cpp
)

target_include_directories(qttest_unit_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
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
