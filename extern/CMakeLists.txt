cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include(FetchContent)

FetchContent_Declare(
  byteme 
  GIT_REPOSITORY https://github.com/LTLA/byteme
  GIT_TAG 1baca1ffddfbf0cd71fa7e285554959e3a116911
)

FetchContent_MakeAvailable(byteme)
