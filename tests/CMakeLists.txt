include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/7da55820cc32dedd6c1b048f2d4e13fdde5e8237.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Avoid installing GoogleTest when installing this project.
option(INSTALL_GTEST "Enable installation of googletest." OFF)

FetchContent_MakeAvailable(googletest)

set(CMAKE_CXX_STANDARD 17)

enable_testing()

add_executable(libtest 
    src/preamble.cpp
    src/integer.cpp
    src/real.cpp
    src/complex.cpp
    src/coordinate_matrix.cpp
    src/coordinate_vector.cpp
    src/array_matrix.cpp
    src/array_vector.cpp
    src/pattern_matrix.cpp
    src/pattern_vector.cpp
    src/from_text.cpp
    src/from_gzip.cpp)

target_link_libraries(libtest 
    gtest_main
    gmock_main
    eminem)

include(CheckIncludeFiles)
check_include_files(filesystem HAVE_CXX_FS)
if (NOT HAVE_CXX_FS)
    target_link_libraries(libtest stdc++fs)
endif()

target_compile_options(libtest PRIVATE -Wall -Wextra -Wpedantic -Werror)

set(CODE_COVERAGE OFF CACHE BOOL "Enable coverage testing")
if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(libtest PRIVATE -O0 -g --coverage)
    target_link_options(libtest PRIVATE --coverage)
endif()

# Making the tests discoverable.
include(GoogleTest)
gtest_discover_tests(libtest)
