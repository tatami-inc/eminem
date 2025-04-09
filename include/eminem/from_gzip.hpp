#ifndef EMINEM_FROM_GZIP_HPP
#define EMINEM_FROM_GZIP_HPP

#include "Parser.hpp"
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
 * @brief Parse a Gzip-compressed Matrix Market file.
 */
class GzipFileParser final : public Parser<byteme::PerByteInterface<char> > { 
public:
    /**
     * @brief Options for the constructor.
     */
    struct Options {
        /**
         * Buffer size to use for reading and decompression.
         */
        size_t buffer_size = 65536;

        /**
         * Whether to parallelize the reading/parsing with `byteme::PerByteParallel`.
         */
        bool parallel = false;
    };

public:
    /**
     * @param path Pointer to a string containing a path to a Gzip-compressed Matrix Market file.
     * @param options Further options.
     */
    GzipFileParser(const char* path, const Options& options) : Parser<byteme::PerByteInterface<char> >(
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
    ) {}
};

/**
 * @brief Parse a Zlib-compressed Matrix Market buffer.
 */
class ZlibBufferParser final : public Parser<byteme::PerByteInterface<char> > {
public:
    /**
     * @brief Options for the constructor.
     */
    struct Options {
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

public:
    /**
     * @param buffer Pointer to an array containing the contents of a Zlib-compressed Matrix Market file.
     * @param len Length of the array referenced by `buffer`.
     * @param options Further options.
     */
    ZlibBufferParser(const unsigned char* buffer, size_t len, const Options& options) : Parser<byteme::PerByteInterface<char> >(
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
    ) {}
};

/**
 * @brief Parse a (possibly Gzip-compressed) Matrix Market file.
 */
class SomeFileParser final : public Parser<byteme::PerByteInterface<char> > {
public:
    /**
     * @brief Options for the constructor.
     */
    struct Options {
        /**
         * Buffer size to use for reading and decompression.
         */
        size_t buffer_size = 65536;

        /**
         * Whether to parallelize the reading/parsing with `byteme::PerByteParallel`.
         */
        bool parallel = false;
    };

public:
    /**
     * @param path Pointer to a string containing a path to a possibly-compressed Matrix Market file.
     * @param options Further options.
     */
    SomeFileParser(const char* path, const Options& options) : Parser<byteme::PerByteInterface<char> >(
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
    ) {}
};

/**
 * @brief Parse a (possibly Zlib-compressed) Matrix Market buffer.
 */
class SomeBufferParser final : public Parser<byteme::PerByteInterface<char> > {
public:
    /**
     * @brief Options for the constructor.
     */
    struct Options {
        /**
         * Buffer size to use for reading and decompression.
         */
        size_t buffer_size = 65536;

        /**
         * Whether to parallelize the reading/parsing with `byteme::PerByteParallel`.
         */
        bool parallel = false;
    };

public:
    /**
     * @param buffer Pointer to an array containing the contents of a possibly-compressed Matrix Market file.
     * @param len Length of the array referenced by `buffer`.
     * @param options Further options.
     */
    SomeBufferParser(const unsigned char* buffer, size_t len, const Options& options) : Parser<byteme::PerByteInterface<char> >(
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
    ) {}
};

}

#endif
