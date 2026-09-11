cmake_minimum_required(VERSION 3.21)
project(qttest_pitch_backend LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT QTTEST_SOURCE_ROOT)
    set(QTTEST_SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

set(PitchRTI_ROOT "$ENV{PitchRTI_ROOT}" CACHE PATH "Pitch pRTI SDK installation root")
if(NOT PitchRTI_ROOT)
    message(FATAL_ERROR "PitchRTI_ROOT is not set. Set it to the pRTI SDK installation directory.")
endif()

find_package(PitchRTI REQUIRED CONFIG
    PATHS "${PitchRTI_ROOT}/cmake"
    NO_DEFAULT_PATH
)

if(NOT TARGET PitchRTI::librti1516e)
    message(FATAL_ERROR "PitchRTI::librti1516e was not provided by the selected Pitch SDK")
endif()

add_library(qttest_hla_pitch1516e_backend SHARED
    "${QTTEST_SOURCE_ROOT}/integrations/hla/pitch1516e/Pitch1516eBackendPlugin.cpp"
    "${QTTEST_SOURCE_ROOT}/src/infrastructure/interoperability/hla/HlaBackendPluginApi.h"
)

target_include_directories(qttest_hla_pitch1516e_backend PRIVATE
    "${QTTEST_SOURCE_ROOT}/src"
)

target_compile_definitions(qttest_hla_pitch1516e_backend PRIVATE
    QTTEST_HLA_1516E_BACKEND_ID="pitch1516e"
    QTTEST_HLA_1516E_BACKEND_NAME="pRTI IEEE 1516e"
)

target_link_libraries(qttest_hla_pitch1516e_backend PRIVATE
    PitchRTI::librti1516e
)

set_target_properties(qttest_hla_pitch1516e_backend PROPERTIES
    OUTPUT_NAME qttest_hla_pitch1516e_backend
    WINDOWS_EXPORT_ALL_SYMBOLS ON
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/$<0:>"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/$<0:>"
)
