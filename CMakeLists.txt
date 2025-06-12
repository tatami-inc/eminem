cmake_minimum_required(VERSION 3.14)

project(eminem
    VERSION 1.1.1
    DESCRIPTION "Matrix Market parser"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(eminem INTERFACE)
add_library(tatami::eminem ALIAS eminem)

option(EMINEM_FETCH_EXTERN "Automatically fetch eminem's external dependencies." ON)
if(EMINEM_FETCH_EXTERN)
    add_subdirectory(extern)
else()
    find_package(ltla_byteme 2.0.0 CONFIG REQUIRED)
endif()

target_link_libraries(eminem INTERFACE ltla::byteme)

# Switch between include directories depending on whether the downstream is
# using the build directly or is using the installed package.
include(GNUInstallDirs)
target_include_directories(eminem
    INTERFACE 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/tatami_eminem>"
)

# Building the test-related machinery, if we are compiling this library directly.
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    option(EMINEM_TESTS "Build eminem's test suite." ON)
else()
    option(EMINEM_TESTS "Build eminem's test suite." OFF)
endif()

if(EMINEM_TESTS)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif() 
    add_subdirectory(perf)
endif()

# Installing for find_package.
include(CMakePackageConfigHelpers)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tatami_eminem)

install(TARGETS eminem 
    EXPORT eminemTargets)

install(EXPORT eminemTargets
    FILE tatami_eminemTargets.cmake
    NAMESPACE tatami::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tatami_eminem)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/tatami_eminemConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tatami_eminem)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/tatami_eminemConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tatami_eminemConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/tatami_eminemConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tatami_eminem)
