include(FetchContent)
find_package(Threads REQUIRED)
FetchContent_Declare(qttest_mixr
    GIT_REPOSITORY https://github.com/doughodson/mixr.git
    GIT_TAG ${QTTEST_MIXR_GIT_TAG}
    GIT_SHALLOW FALSE
    PATCH_COMMAND ${CMAKE_COMMAND} -DMIXR_SOURCE_DIR=<SOURCE_DIR>
        -P ${CMAKE_CURRENT_LIST_DIR}/ApplyMixrPatch.cmake)
FetchContent_MakeAvailable(qttest_mixr)

# Use upstream's explicit source manifests, compiling with the application's
# toolchain and runtime. In particular Windows never invokes GNU make/MinGW.
foreach(component base simulation models)
    file(READ "${qttest_mixr_SOURCE_DIR}/src/${component}/Makefile" manifest)
    string(REGEX MATCHALL "[A-Za-z0-9_/]+\\.o" objects "${manifest}")
    list(REMOVE_DUPLICATES objects)
    set(sources)
    foreach(object IN LISTS objects)
        string(REGEX REPLACE "\\.o$" ".cpp" source "${object}")
        if(WIN32)
            string(REPLACE "_linux.cpp" "_msvc.cpp" source "${source}")
        endif()
        list(APPEND sources "${qttest_mixr_SOURCE_DIR}/src/${component}/${source}")
    endforeach()
    add_library(qttest_mixr_${component} STATIC ${sources})
    target_include_directories(qttest_mixr_${component} PUBLIC
        "${qttest_mixr_SOURCE_DIR}/include"
        "${qttest_mixr_SOURCE_DIR}/thirdparty/include")
    target_compile_features(qttest_mixr_${component} PUBLIC cxx_std_17)
    set_target_properties(qttest_mixr_${component} PROPERTIES POSITION_INDEPENDENT_CODE ON)
    if(MSVC)
        target_compile_definitions(qttest_mixr_${component} PRIVATE
            _CRT_SECURE_NO_WARNINGS _USE_MATH_DEFINES NOMINMAX)
    elseif(APPLE)
        target_compile_options(qttest_mixr_${component} PRIVATE -Wno-register)
    endif()
endforeach()
target_link_libraries(qttest_mixr_base PUBLIC Threads::Threads)
if(WIN32)
    target_link_libraries(qttest_mixr_base PUBLIC ws2_32 winmm)
endif()
target_link_libraries(qttest_mixr_simulation PUBLIC qttest_mixr_base)
target_link_libraries(qttest_mixr_models PUBLIC qttest_mixr_simulation)
add_library(qttest_mixr_sensor SHARED
    ${CMAKE_CURRENT_LIST_DIR}/../integrations/mixr/MixrSensorPlugin.cpp)
target_include_directories(qttest_mixr_sensor PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../src)
target_link_libraries(qttest_mixr_sensor PRIVATE qttest_mixr_models)
target_compile_definitions(qttest_mixr_sensor PRIVATE
    QTTEST_MIXR_PROVIDER_VERSION="${QTTEST_MIXR_GIT_TAG}")
set_target_properties(qttest_mixr_sensor PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS ON
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/sensor-plugins/$<0:>"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/sensor-plugins/$<0:>")
add_executable(qttest_mixr_smoke ${CMAKE_CURRENT_LIST_DIR}/../integrations/mixr/MixrSmoke.cpp)
target_include_directories(qttest_mixr_smoke PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../src)
target_link_libraries(qttest_mixr_smoke PRIVATE Qt6::Core)
add_dependencies(qttest_mixr_smoke qttest_mixr_sensor)
