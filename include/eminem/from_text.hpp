#ifndef EMINEM_FROM_TEXT_HPP
#define EMINEM_FROM_TEXT_HPP

#include "Parser.hpp"
#include "byteme/RawFileReader.hpp"
#include "byteme/RawBufferReader.hpp"
#include <memory>

/**
 * @file from_text.hpp
 * @brief Read a Matrix Market file from text.
 */

namespace eminem {

/**
 * @brief Parse a Matrix Market text file.
 *
 * @tparam parallel_ Whether to parallelize the reading/parsing, see `Parser`.
 */
template<bool parallel_ = false>
struct TextFileParser : public Parser<parallel_, std::unique_ptr<byteme::RawFileReader> > { 
public:
    /**
     * @param path Pointer to a string containing a path to an uncompressed Matrix Market file.
     * @param chunk_size Chunk size to use for reading.
     */
    TextFileParser(const char* path, size_t chunk_size = 65536) : 
        Parser<parallel_, std::unique_ptr<byteme::RawFileReader> >(std::make_unique<byteme::RawFileReader>(path, chunk_size)) {}
};

/**
 * @brief Parse a Matrix Market text buffer.
 *
 * @tparam parallel_ Whether to parallelize the reading/parsing, see `Parser`.
 */
template<bool parallel_ = false>
struct TextBufferParser : public Parser<parallel_, std::unique_ptr<byteme::RawBufferReader> > { 
public:
    /**
     * @param buffer Pointer to an array containing the contents of an uncompressed Matrix Market file.
     * @param len Length of the array referenced by `buffer`.
     */
    TextBufferParser(const unsigned char* buffer, size_t len) : 
        Parser<parallel_, std::unique_ptr<byteme::RawBufferReader> >(std::make_unique<byteme::RawBufferReader>(buffer, len)) {}

    /**
     * @param buffer Pointer to an array containing the contents of an uncompressed Matrix Market file.
     * @param len Length of the array referenced by `buffer`.
     */
    TextBufferParser(const char* buffer, size_t len) : TextBufferParser(reinterpret_cast<const unsigned char*>(buffer), len) {}
};




}

#endif
