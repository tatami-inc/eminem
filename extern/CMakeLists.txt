cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include(FetchContent)

FetchContent_Declare(
  byteme 
  GIT_REPOSITORY https://github.com/LTLA/byteme
  GIT_TAG 31c0674e26e4d5619818b7a356c3753c6d205db9
)

FetchContent_MakeAvailable(byteme)
