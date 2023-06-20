#ifndef EMINEM_FROM_GZIP_HPP
#define EMINEM_FROM_GZIP_HPP

#include "Parser.hpp"
#include "byteme/GzipFileReader.hpp"
#include "byteme/ZlibBufferReader.hpp"
#include "byteme/SomeFileReader.hpp"
#include "byteme/SomeBufferReader.hpp"
#include <memory>

/**
 * @file from_text.hpp
 * @brief Read a Matrix Market file from text.
 */

namespace eminem {

/**
 * @brief Parse a Gzip-compressed Matrix Market file.
 *
 * @tparam parallel_ Whether to parallelize the reading/parsing, see `Parser`.
 */
template<bool parallel_ = false>
struct GzipFileParser : public Parser<parallel_, std::unique_ptr<byteme::GzipFileReader> > { 
public:
    /**
     * @param path Pointer to a string containing a path to a Gzip-compressed Matrix Market file.
     * @param buffer_size Buffer size to use for reading.
     */
    GzipFileParser(const char* path, size_t buffer_size = 65536) : 
        Parser<parallel_, std::unique_ptr<byteme::GzipFileReader> >(std::make_unique<byteme::GzipFileReader>(path, buffer_size)) {}
};

/**
 * @brief Parse a Zlib-compressed Matrix Market buffer.
 *
 * @tparam parallel_ Whether to parallelize the reading/parsing, see `Parser`.
 */
template<bool parallel_ = false>
struct ZlibBufferParser : public Parser<parallel_, std::unique_ptr<byteme::ZlibBufferReader> > { 
public:
    /**
     * @param buffer Pointer to an array containing the contents of a Zlib-compressed Matrix Market file.
     * @param len Length of the array referenced by `buffer`.
     * @param mode Compression of the stream, see the `byteme::ZlibBufferReader` constructor.
     * @param buffer_size Size of the buffer to use for reading.
     */
    ZlibBufferParser(const unsigned char* buffer, size_t len, int mode = 3, size_t buffer_size = 65536) : 
        Parser<parallel_, std::unique_ptr<byteme::ZlibBufferReader> >(std::make_unique<byteme::ZlibBufferReader>(buffer, len, mode, buffer_size)) {}
};

/**
 * @brief Parse a (possibly Gzip-compressed) Matrix Market file.
 *
 * @tparam parallel_ Whether to parallelize the reading/parsing, see `Parser`.
 */
template<bool parallel_ = false>
struct SomeFileParser : public Parser<parallel_, std::unique_ptr<byteme::SomeFileReader> > { 
public:
    /**
     * @param path Pointer to a string containing a path to a possibly-compressed Matrix Market file.
     * @param buffer_size Buffer size to use for reading.
     */
    SomeFileParser(const char* path, size_t buffer_size = 65536) : 
        Parser<parallel_, std::unique_ptr<byteme::SomeFileReader> >(std::make_unique<byteme::SomeFileReader>(path, buffer_size)) {}
};

/**
 * @brief Parse a (possibly Zlib-compressed) Matrix Market buffer.
 *
 * @tparam parallel_ Whether to parallelize the reading/parsing, see `Parser`.
 */
template<bool parallel_ = false>
struct SomeBufferParser : public Parser<parallel_, std::unique_ptr<byteme::SomeBufferReader> > { 
public:
    /**
     * @param buffer Pointer to an array containing the contents of a possibly-compressed Matrix Market file.
     * @param len Length of the array referenced by `buffer`.
     * @param buffer_size Size of the buffer to use for reading.
     */
    SomeBufferParser(const unsigned char* buffer, size_t len, size_t buffer_size = 65536) : 
        Parser<parallel_, std::unique_ptr<byteme::SomeBufferReader> >(std::make_unique<byteme::SomeBufferReader>(buffer, len, buffer_size)) {}
};

}

#endif
