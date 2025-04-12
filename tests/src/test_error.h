#ifndef TEST_ERROR_H
#define TEST_ERROR_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>
#include <memory>

#include "byteme/byteme.hpp"
#include "eminem/Parser.hpp"

inline void test_error(const std::string& input, std::string msg) {
    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
    EXPECT_ANY_THROW({
        try {
            parser.scan_preamble();
        } catch (std::exception& e) {
            EXPECT_THAT(e.what(), ::testing::HasSubstr(msg));
            throw;
        }
    });
}

#endif
