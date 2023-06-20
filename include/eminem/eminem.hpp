#ifndef EMINEM_EMINEM_HPP
#define EMINEM_EMINEM_HPP

#include "Parser.hpp"
#include "from_text.hpp"

#ifndef EMINEM_NO_ZLIB
#include "from_gzip.hpp"
#endif

/**
 * @file eminem.hpp
 * @brief Umbrella header for the **eminem** library.
 * 
 * Developers should define `EMINEM_NO_ZLIB` when Zlib is not available.
 * This will avoid including the parsers for Gzip-compressed files.
 */

/**
 * @namespace eminem
 * @brief Classes and methods for parsing Matrix Market files.
 */
namespace eminem {}

#endif
