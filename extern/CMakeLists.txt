cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include(FetchContent)

FetchContent_Declare(
  byteme 
  GIT_REPOSITORY https://github.com/LTLA/byteme
  GIT_TAG 5b193699af4e9fd59ea012d5036ed5cdf8c884b6
)

FetchContent_MakeAvailable(byteme)
