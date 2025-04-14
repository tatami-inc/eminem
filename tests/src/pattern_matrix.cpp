#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "byteme/byteme.hpp"
#include "eminem/Parser.hpp"

#include <string>
#include <memory>
#include <cstdint>
#include <limits>

class ParserPatternMatrixTest : public ::testing::TestWithParam<std::tuple<std::string, int, int, std::vector<int>, std::vector<int> > > {};

TEST_P(ParserPatternMatrixTest, Success) {
    auto param = GetParam();
    const std::string& content = std::get<0>(param);
    int nr = std::get<1>(param);
    int nc = std::get<2>(param);
    const auto& expected_r = std::get<3>(param);
    const auto& expected_c = std::get<4>(param);

    std::string input = "%%MatrixMarket matrix coordinate pattern general\n" + std::to_string(nr) + " " + std::to_string(nc) + " " + std::to_string(expected_r.size()) + "\n" + content;
    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
    parser.scan_preamble();

    const auto& deets = parser.get_banner();
    EXPECT_EQ(deets.object, eminem::Object::MATRIX);
    EXPECT_EQ(deets.format, eminem::Format::COORDINATE);
    EXPECT_EQ(deets.field, eminem::Field::PATTERN);
    EXPECT_EQ(deets.symmetry, eminem::Symmetry::GENERAL);

    EXPECT_EQ(parser.get_nrows(), nr);
    EXPECT_EQ(parser.get_ncols(), nc);
    EXPECT_EQ(parser.get_nlines(), expected_r.size());

    std::vector<int> observed_r, observed_c;
    EXPECT_TRUE(parser.scan_pattern([&](size_t r, size_t c, bool val) -> void {
        observed_r.push_back(r);
        observed_c.push_back(c);
        EXPECT_TRUE(val);
    }));
    EXPECT_EQ(observed_r, expected_r);
    EXPECT_EQ(observed_c, expected_c);
}

INSTANTIATE_TEST_SUITE_P(
    ParserPatternMatrix,
    ParserPatternMatrixTest,
    ::testing::Values(
        std::make_tuple<std::string, int, int, std::vector<int>, std::vector<int> >(
            "1 11\n22 2\n3 333\n444 4\n", // trailing newline
            500,
            500,
            { 1, 22, 3, 444 },
            { 11, 2, 333, 4 }
        ),
        std::make_tuple<std::string, int, int, std::vector<int>, std::vector<int> >(
            "81 19\n29 122\n39 37\n494 96", // no trailing newline
            500,
            500,
            { 81, 29, 39, 494 },
            { 19, 122, 37, 96 }
        ),
        std::make_tuple<std::string, int, int, std::vector<int>, std::vector<int> >(
            "191 121\n422 102\n73 33\n484 46   ", // trailing blank but no trailing newline
            500,
            500,
            { 191, 422, 73, 484 },
            { 121, 102, 33, 46 }
        ),
        std::make_tuple<std::string, int, int, std::vector<int>, std::vector<int> >(
            "   1 11   \n\t22 \t2   \t\n3\t\t333     \n444  4   \n", // variable numbers of blanks
            500,
            500,
            { 1, 22, 3, 444 },
            { 11, 2, 333, 4 }
        ),
        std::make_tuple<std::string, int, int, std::vector<int>, std::vector<int> >(
            "\n\n19 29\n%iamacomment\n28 3\n%another comment\n\n65 44\n98 7\n", // comments, newlines and crap. 
            500,
            500,
            { 19, 28, 65, 98 },
            { 29, 3, 44, 7}
        ),
        std::make_tuple<std::string, int, int, std::vector<int>, std::vector<int> >(
            "19 29\n28 3\n65 44\n98 7\n%this is the end", // ends on a comment
            100,
            100,
            { 19, 28, 65, 98 },
            { 29, 3, 44, 7}
        )
    )
);

static void test_error(const std::string& input, std::string msg) {
    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
    parser.scan_preamble();
    EXPECT_ANY_THROW({
        try {
            parser.scan_pattern([&](size_t, size_t, int){});
        } catch (std::exception& e) {
            EXPECT_THAT(e.what(), ::testing::HasSubstr(msg));
            throw;
        }
    });
}

TEST(ParserPatternMatrix, Errors) {
    test_error("%%MatrixMarket matrix coordinate pattern general\n1 1 1\n ", "expected two fields");
    test_error("%%MatrixMarket matrix coordinate pattern general\n1 1 1\n1", "unexpected end of file");
    test_error("%%MatrixMarket matrix coordinate pattern general\n1 1 1\n1 ", "unexpected end of file");
    test_error("%%MatrixMarket matrix coordinate pattern general\n1 1 1\n1\n", "unexpected newline");
    test_error("%%MatrixMarket matrix coordinate pattern general\n1 1 1\n1 \n", "empty index field");
    test_error("%%MatrixMarket matrix coordinate pattern general\n1 1 1\n1 1 1\n", "expected newline");

    test_error("%%MatrixMarket matrix coordinate pattern general\n2 2 1\n1 1\n2 2", "more lines present");
    test_error("%%MatrixMarket matrix coordinate pattern general\n1 1 1\n0 1", "row index must be positive");
    test_error("%%MatrixMarket matrix coordinate pattern general\n1 1 1\n2 1", "row index out of range");
    test_error("%%MatrixMarket matrix coordinate pattern general\n1 1 1\n1 0", "column index must be positive");
    test_error("%%MatrixMarket matrix coordinate pattern general\n1 1 1\n1 2", "column index out of range");

    test_error("%%MatrixMarket matrix coordinate pattern general\n2 2 2\n1 1\n", "fewer lines present");

    {
        std::string input = "%%MatrixMarket matrix coordinate pattern general\n1 1 1\n1 1";
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
        EXPECT_ANY_THROW({
            try {
                parser.scan_pattern([&](size_t, size_t, int) -> void {});
            } catch (std::exception& e) {
                EXPECT_THAT(e.what(), ::testing::HasSubstr("banner or size lines have not yet been parsed"));
                throw;
            }
        });
    }

    {
        std::string input = "%%MatrixMarket matrix array pattern general\n1 1\n1 1";
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
        parser.scan_preamble();
        EXPECT_ANY_THROW({
            try {
                parser.scan_pattern([&](size_t, size_t, bool) -> void {});
            } catch (std::exception& e) {
                EXPECT_THAT(e.what(), ::testing::HasSubstr("not supported"));
                throw;
            }
        });
    }
}

TEST(ParserPatternMatrix, QuitEarly) {
    std::string input = "%%MatrixMarket matrix coordinate pattern general\n10 10 3\n1 2\n4 5\n7 8\n";

    { // quits immediately.
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
        parser.scan_preamble();

        std::vector<int> observed_r, observed_c;
        EXPECT_FALSE(parser.scan_pattern([&](size_t r, size_t c, bool) -> bool {
            observed_r.push_back(r);
            observed_c.push_back(c);
            return false;
        }));
        std::vector<int> expected_r{ 1 }, expected_c{ 2 };
        EXPECT_EQ(observed_r, expected_r);
        EXPECT_EQ(observed_c, expected_c);
    }

    { // never quits but function still returns a value.
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
        parser.scan_preamble();

        std::vector<int> observed_r, observed_c;
        EXPECT_TRUE(parser.scan_pattern([&](size_t r, size_t c, bool) -> bool {
            observed_r.push_back(r);
            observed_c.push_back(c);
            return true;
        }));
        std::vector<int> expected_r{ 1, 4, 7 }, expected_c{ 2, 5, 8 };
        EXPECT_EQ(observed_r, expected_r);
        EXPECT_EQ(observed_c, expected_c);
    }
}

TEST(ParserPatternMatrix, Empty) {
    std::string input = "%%MatrixMarket matrix coordinate pattern general\n0 0 0";
    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));

    parser.scan_preamble();

    const auto& deets = parser.get_banner();
    EXPECT_EQ(deets.object, eminem::Object::MATRIX);
    EXPECT_EQ(deets.format, eminem::Format::COORDINATE);
    EXPECT_EQ(deets.field, eminem::Field::PATTERN);
    EXPECT_EQ(deets.symmetry, eminem::Symmetry::GENERAL);

    EXPECT_EQ(parser.get_nrows(), 0);
    EXPECT_EQ(parser.get_ncols(), 0);
    EXPECT_EQ(parser.get_nlines(), 0);

    std::vector<bool> observed;
    EXPECT_TRUE(parser.scan_pattern([&](size_t, size_t, bool val) -> void {
        observed.push_back(val);
    }));
    EXPECT_TRUE(observed.empty());
}
