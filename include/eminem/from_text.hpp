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
 */
class TextFileParser : public Parser<byteme::PerByteInterface<char> > {
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
     * @param path Pointer to a string containing a path to an uncompressed Matrix Market file.
     * @param options Further options.
     */
    TextFileParser(const char* path, const Options& options) : Parser<byteme::PerByteInterface<char> >(
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
    ) {}
};

/**
 * @brief Parse a Matrix Market text buffer.
 */
class TextBufferParser : public Parser<byteme::PerByteInterface<char> > {
public:
    /**
     * @brief Options for the constructor.
     */
    struct Options {
        /**
         * Whether to parallelize the reading/parsing with `byteme::PerByteParallel`.
         */
        bool parallel = false;
    };

public:
    /**
     * @param buffer Pointer to an array containing the contents of an uncompressed Matrix Market file.
     * @param len Length of the array referenced by `buffer`.
     * @param options Further options.
     */
    TextBufferParser(const unsigned char* buffer, size_t len, const Options& options) : Parser<byteme::PerByteInterface<char> >(
        [&]() -> std::unique_ptr<byteme::PerByteInterface<char> > {
            auto reader = std::make_unique<byteme::RawBufferReader>(buffer, len);
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
