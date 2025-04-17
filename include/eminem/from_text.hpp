#ifndef EMINEM_FROM_TEXT_HPP
#define EMINEM_FROM_TEXT_HPP

#include <memory>
#include <cstddef>

#include "Parser.hpp"
#include "byteme/PerByte.hpp"
#include "byteme/RawFileReader.hpp"
#include "byteme/RawBufferReader.hpp"

/**
 * @file from_text.hpp
 * @brief Read a Matrix Market file from text.
 */

namespace eminem {

/**
 * @brief Options for `parse_text_file()`.
 */
struct ParseTextFileOptions {
    /**
     * Buffer size to use for reading and decompression.
     */
    std::size_t buffer_size = 65536;

    /**
     * Whether to parallelize the reading/parsing with `byteme::PerByteParallel`.
     */
    bool parallel = false;
};

/**
 * Parse a Matrix Market text file.
 * @param path Pointer to a string containing a path to an uncompressed Matrix Market file.
 * @param options Further options.
 */
inline Parser<byteme::PerByteInterface<char> > parse_text_file(const char* path, const ParseTextFileOptions& options) {
    return Parser<byteme::PerByteInterface<char> >(
        [&]() -> std::unique_ptr<byteme::PerByteInterface<char> > {
            byteme::RawFileReaderOptions topt;
            topt.buffer_size = options.buffer_size;
            auto reader = std::make_unique<byteme::RawFileReader>(path, topt);
            if (options.parallel) {
                return std::make_unique<byteme::PerByteParallel<char> >(std::move(reader));
            } else {
                return std::make_unique<byteme::PerByteSerial<char> >(std::move(reader));
            }
        }()
    );
}

/**
 * @brief Options for `parse_text_buffer()`.
 */
struct ParseTextBufferOptions {
    /**
     * Whether to parallelize the reading/parsing with `byteme::PerByteParallel`.
     */
    bool parallel = false;
};

/**
 * Parse a Matrix Market text buffer.
 * @param buffer Pointer to an array containing the contents of an uncompressed Matrix Market file.
 * @param len Length of the array referenced by `buffer`.
 * @param options Further options.
 */
inline Parser<byteme::PerByteInterface<char> > parse_text_buffer(const unsigned char* buffer, std::size_t len, const ParseTextBufferOptions& options) {
    return Parser<byteme::PerByteInterface<char> >(
        [&]() -> std::unique_ptr<byteme::PerByteInterface<char> > {
            auto reader = std::make_unique<byteme::RawBufferReader>(buffer, len);
            if (options.parallel) {
                return std::make_unique<byteme::PerByteParallel<char> >(std::move(reader));
            } else {
                return std::make_unique<byteme::PerByteSerial<char> >(std::move(reader));
            }
        }()
    );
}

}

#endif
