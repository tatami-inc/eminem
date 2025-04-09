#ifndef EMINEM_FROM_GZIP_HPP
#define EMINEM_FROM_GZIP_HPP

#include "Parser.hpp"
#include "byteme/PerByte.hpp"
#include "byteme/GzipFileReader.hpp"
#include "byteme/ZlibBufferReader.hpp"
#include "byteme/SomeFileReader.hpp"
#include "byteme/SomeBufferReader.hpp"
#include <memory>

/**
 * @file from_gzip.hpp
 * @brief Read a Gzipped Matrix Market file.
 */

namespace eminem {

/**
 * @brief Options for `parse_gzip_file()`.
 */
struct ParseGzipFileOptions {
    /**
     * Buffer size to use for reading and decompression.
     */
    size_t buffer_size = 65536;

    /**
     * Whether to parallelize the reading/parsing with `byteme::PerByteParallel`.
     */
    bool parallel = false;
};

/**
 * Parse a Gzip-compressed Matrix Market file.
 * @param path Pointer to a string containing a path to a Gzip-compressed Matrix Market file.
 * @param options Further options.
 */
inline Parser<byteme::PerByteInterface<char> > parse_gzip_file(const char* path, const ParseGzipFileOptions& options) {
    return Parser<byteme::PerByteInterface<char> >(
        [&]() -> std::unique_ptr<byteme::PerByteInterface<char> > {
            byteme::GzipFileReaderOptions gopt;
            gopt.buffer_size = options.buffer_size;
            auto reader = std::make_unique<byteme::GzipFileReader>(path, gopt);
            if (options.parallel) {
                return std::make_unique<byteme::PerByteParallel<char> >(std::move(reader));
            } else {
                return std::make_unique<byteme::PerByteSerial<char> >(std::move(reader));
            }
        }()
    );
}

/**
 * @brief Options for `parse_zlib_buffer()`.
 */
struct ParseZlibBufferOptions {
    /**
     * Buffer size to use for reading.
     */
    size_t buffer_size = 65536;

    /**
     * Whether to parallelize the reading/parsing with `byteme::PerByteParallel`.
     */
    bool parallel = false;

    /**
     * Compression of the stream, i.e., DEFLATE, zlib or gzip.
     * This is set to 3 to perform auto-detection, see the `byteme::ZlibBufferReader` constructor.
     */
    int mode = 3;
};

/**
 * Parse a Zlib-compressed Matrix Market buffer.
 * @param buffer Pointer to an array containing the contents of a Zlib-compressed Matrix Market file.
 * @param len Length of the array referenced by `buffer`.
 * @param options Further options.
 */
inline Parser<byteme::PerByteInterface<char> > parse_zlib_buffer(const unsigned char* buffer, size_t len, const ParseZlibBufferOptions& options) {
    return Parser<byteme::PerByteInterface<char> >(
        [&]() -> std::unique_ptr<byteme::PerByteInterface<char> > {
            byteme::ZlibBufferReaderOptions zopt;
            zopt.buffer_size = options.buffer_size;
            zopt.mode = options.mode;
            auto reader = std::make_unique<byteme::ZlibBufferReader>(buffer, len, zopt);
            if (options.parallel) {
                return std::make_unique<byteme::PerByteParallel<char> >(std::move(reader));
            } else {
                return std::make_unique<byteme::PerByteSerial<char> >(std::move(reader));
            }
        }()
    );
}

/**
 * @brief Options for `parse_some_file()`.
 */
struct ParseSomeFileOptions {
    /**
     * Buffer size to use for reading and decompression.
     */
    size_t buffer_size = 65536;

    /**
     * Whether to parallelize the reading/parsing with `byteme::PerByteParallel`.
     */
    bool parallel = false;
};

/**
 * Parse a possibly Gzip-compressed or uncompressed Matrix Market file.
 * @param path Pointer to a string containing a path to a possibly-compressed Matrix Market file.
 * @param options Further options.
 */
inline Parser<byteme::PerByteInterface<char> > parse_some_file(const char* path, const ParseSomeFileOptions& options) {
    return Parser<byteme::PerByteInterface<char> >(
        [&]() -> std::unique_ptr<byteme::PerByteInterface<char> > {
            byteme::SomeFileReaderOptions sopt;
            sopt.buffer_size = options.buffer_size;
            auto reader = std::make_unique<byteme::SomeFileReader>(path, sopt);
            if (options.parallel) {
                return std::make_unique<byteme::PerByteParallel<char> >(std::move(reader));
            } else {
                return std::make_unique<byteme::PerByteSerial<char> >(std::move(reader));
            }
        }()
    );
}

/**
 * @brief Options for `parse_some_buffer()`.
 */
struct ParseSomeBufferOptions {
    /**
     * Buffer size to use for reading and decompression.
     */
    size_t buffer_size = 65536;

    /**
     * Whether to parallelize the reading/parsing with `byteme::PerByteParallel`.
     */
    bool parallel = false;
};

/**
 * Parse a possibly Zlib-compressed or uncompressed Matrix Market buffer.
 * @param buffer Pointer to an array containing the contents of a possibly-compressed Matrix Market file.
 * @param len Length of the array referenced by `buffer`.
 * @param options Further options.
 */
inline Parser<byteme::PerByteInterface<char> > parse_some_buffer(const unsigned char* buffer, size_t len, const ParseSomeBufferOptions& options) {
    return Parser<byteme::PerByteInterface<char> >(
        [&]() -> std::unique_ptr<byteme::PerByteInterface<char> > {
            byteme::SomeBufferReaderOptions sopt;
            sopt.buffer_size = options.buffer_size;
            auto reader = std::make_unique<byteme::SomeBufferReader>(buffer, len, sopt);
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
