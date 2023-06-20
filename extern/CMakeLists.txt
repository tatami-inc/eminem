cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include(FetchContent)

FetchContent_Declare(
  byteme 
  GIT_REPOSITORY https://github.com/LTLA/byteme
  GIT_TAG 0d1f128a7c07d3739ccc65e39758f209ede834a9
)

FetchContent_MakeAvailable(byteme)
