# Parse Matrix Market files in C++

![Unit tests](https://github.com/tatami-inc/eminem/actions/workflows/run-tests.yaml/badge.svg)
![Documentation](https://github.com/tatami-inc/eminem/actions/workflows/doxygenate.yaml/badge.svg)
[![codecov](https://codecov.io/gh/tatami-inc/eminem/branch/master/graph/badge.svg?token=7I3UBJLHSO)](https://codecov.io/gh/tatami-inc/eminem)

## Overview

The **eminem** library implements parsers for Matrix Market (`*.mm`/`*.mtx`) files.
We support matrix or vector objects, coordinate or array formats, and all the different fields (integer, real, complex and pattern).
We support reading from text files, Gzip-compressed files, as well as in-memory buffers.
Users can supply their own functions to process each line on the fly, based on the information in the banner and size lines.

## Quick start

We first create a `Parser` instance, in this case from a text file.
We also support parsing of Gzip-compressed files, buffers, or any input source of bytes compatible with the [**byteme**](https://github.com/LTLA/byteme) interfaces.

```cpp
#include "eminem/eminem.hpp"

// Using default options, hence the '{}'
auto parser = eminem::parse_text_file("some_path.mm", {});
```

We scan through the preamble, which contains the banner and the size lines.
The type of the data field in the banner will determine which scanning function to use.
The size lines can also help with pre-allocation of memory to store the scanning output, if necessary.

```cpp
parser.scan_preamble();
auto deets = parser.get_banner();
auto NR = parser.get_nrows();
auto NC = parser.get_ncols();
auto NL = parser.get_nlines();
```

Finally, we scan through the actual data lines, providing a lambda or functor to process each row/column/value triplet as it is parsed.
Note that the row/column indices are 1-based when they are passed to the lambda.

```cpp
if (deets.field == eminem::Field::INTEGER) {
    parser.scan_integer([&](size_t r, size_t c, int v) -> void {
        // Do something per line.
    });
} else if (deets.field == eminem::Field::REAL) {
    parser.scan_real([&](size_t r, size_t c, double v) -> void {
        // Do something else.
    });
} else {
    // and so on...
}
```

Check out the [reference documentation](https://tatami-inc.github.io/eminem/) for more details.

## Building projects

### CMake with `FetchContent`

If you're using CMake, you just need to add something like this to your `CMakeLists.txt`:

```cmake
include(FetchContent)

FetchContent_Declare(
  tatami
  GIT_REPOSITORY https://github.com/tatami-inc/eminem
  GIT_TAG master # or any version of interest
)

FetchContent_MakeAvailable(tatami)
```

Then you can link to **tatami** to make the headers available during compilation:

```cmake
# For executables:
target_link_libraries(myexe eminem)

# For libaries
target_link_libraries(mylib INTERFACE eminem)
```

### CMake using `find_package()`

You can install the library by cloning a suitable version of this repository and running the following commands:

```sh
mkdir build && cd build
cmake .. -DEMINEM_TESTS=OFF
cmake --build . --target install
```

Then you can use `find_package()` as usual:

```cmake
find_package(tatami_eminem CONFIG REQUIRED)
target_link_libraries(mylib INTERFACE tatami::eminem)
```

### Manual

If you're not using CMake, the simple approach is to just copy the files the `include/` subdirectory -
either directly or with Git submodules - and include their path during compilation with, e.g., GCC's `-I`.

You'll also need to add the [**byteme**](https://github.com/LTLA/byteme) header-only library to the compiler's search path.
Normally, when using CMake, this is automatically linked to Zlib; this will now need to be done manually.
