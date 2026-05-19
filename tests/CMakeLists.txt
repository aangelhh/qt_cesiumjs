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
    application/EventBusTest.cpp
    presentation/EntityVisualStateManagerTest.cpp
    presentation/EntityHomePositionTrackerTest.cpp
    # Sources under test. We re-compile them here rather than link against the
    # qttest executable target (which is a Qt application bundle). When the
    # core code is moved into a static library this list will collapse into a
    # single target_link_libraries() call.
    ${CMAKE_SOURCE_DIR}/src/domain/Task.cpp
    ${CMAKE_SOURCE_DIR}/src/domain/GeoMath.cpp
    ${CMAKE_SOURCE_DIR}/src/domain/BombReleaseGate.cpp
    ${CMAKE_SOURCE_DIR}/src/domain/CombatRules.cpp
    ${CMAKE_SOURCE_DIR}/src/presentation/EntityVisualStateManager.cpp
    ${CMAKE_SOURCE_DIR}/src/presentation/EntityHomePositionTracker.cpp
)

target_include_directories(qttest_unit_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(qttest_unit_tests
    PRIVATE
        Qt6::Core
        GTest::gtest
        GTest::gtest_main
)

if(NOT MSVC)
    target_compile_options(qttest_unit_tests PRIVATE -Wall -Wextra -Wno-unused-parameter)
endif()

include(GoogleTest)
gtest_discover_tests(qttest_unit_tests)
