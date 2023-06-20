# Creating an executable to do performance tests for implementation changes.

add_executable(read_mm src/read_mm.cpp)

find_package(ZLIB)
target_link_libraries(read_mm eminem ZLIB::ZLIB)

target_compile_options(read_mm PRIVATE -O3)
