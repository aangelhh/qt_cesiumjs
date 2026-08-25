add_executable(qttest_jsbsim_runtime_benchmark EXCLUDE_FROM_ALL
    JsbsimRuntimeBenchmark.cpp
    ${CMAKE_SOURCE_DIR}/src/application/FlightDynamicsEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/application/dynamics/KinematicDynamicsModel.cpp
    ${CMAKE_SOURCE_DIR}/src/application/dynamics/JSBSimDynamicsModel.cpp
    ${CMAKE_SOURCE_DIR}/src/application/JsbsimSetpointController.cpp
    ${CMAKE_SOURCE_DIR}/src/application/MovementIntent.cpp
    ${CMAKE_SOURCE_DIR}/src/application/SystemsTelemetry.cpp
    ${CMAKE_SOURCE_DIR}/src/domain/GeoMath.cpp
    ${CMAKE_SOURCE_DIR}/src/domain/Task.cpp
)

target_include_directories(qttest_jsbsim_runtime_benchmark PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_compile_definitions(qttest_jsbsim_runtime_benchmark PRIVATE
    QTTEST_HAS_JSBSIM=1
    QTTEST_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
    QTTEST_BENCHMARK_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
)

target_link_libraries(qttest_jsbsim_runtime_benchmark PRIVATE
    Qt6::Core
    libJSBSim
)

if(NOT MSVC)
    target_compile_options(qttest_jsbsim_runtime_benchmark PRIVATE
        -Wall
        -Wextra
        -Wno-unused-parameter
    )
endif()
