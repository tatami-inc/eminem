#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "byteme/byteme.hpp"
#include "eminem/Parser.hpp"

#include <string>
#include <memory>

#include "simulate.h"
#include "format.h"

class ParserPatternVectorTest : public ::testing::TestWithParam<std::tuple<std::string, int, std::vector<int> > > {};

TEST_P(ParserPatternVectorTest, Success) {
    auto param = GetParam();
    const std::string& content = std::get<0>(param);
    int nr = std::get<1>(param);
    const auto& expected_r = std::get<2>(param);

    std::string input = "%%MatrixMarket vector coordinate pattern general\n" + std::to_string(nr) + " " + std::to_string(expected_r.size()) + "\n" + content;
    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
    parser.scan_preamble();

    const auto& deets = parser.get_banner();
    EXPECT_EQ(deets.object, eminem::Object::VECTOR);
    EXPECT_EQ(deets.format, eminem::Format::COORDINATE);
    EXPECT_EQ(deets.field, eminem::Field::PATTERN);
    EXPECT_EQ(deets.symmetry, eminem::Symmetry::GENERAL);

    EXPECT_EQ(parser.get_nrows(), nr);
    EXPECT_EQ(parser.get_ncols(), 1);
    EXPECT_EQ(parser.get_nlines(), expected_r.size());

    std::vector<int> observed_r;
    EXPECT_TRUE(parser.scan_pattern([&](size_t r, size_t c, bool val) -> void {
        observed_r.push_back(r);
        EXPECT_EQ(c, 1);
        EXPECT_TRUE(val);
    }));
    EXPECT_EQ(observed_r, expected_r);
}

INSTANTIATE_TEST_SUITE_P(
    ParserPatternVector,
    ParserPatternVectorTest,
    ::testing::Values(
        std::make_tuple<std::string, int, std::vector<int> >(
            "11\n22\n33\n44\n", // trailing newline
            50,
            { 11, 22, 33, 44 }
        ),
        std::make_tuple<std::string, int, std::vector<int> >(
            "81\n29\n39\n96", // no trailing newline
            100,
            { 81, 29, 39, 96 }
        ),
        std::make_tuple<std::string, int, std::vector<int> >(
            "191\n422\n73\n484    ", // trailing blank but no trailing newline
            500,
            { 191, 422, 73, 484 }
        ),
        std::make_tuple<std::string, int, std::vector<int> >(
            "   1   \n\t22 \t   \t\n3\t\t     \n444     \n", // variable numbers of blanks
            500,
            { 1, 22, 3, 444 }
        ),
        std::make_tuple<std::string, int, std::vector<int> >(
            "\n\n199\n%iamacomment\n283\n%another comment\n\n654\n987\n", // comments, newlines and crap. 
            1000,
            { 199, 283, 654, 987 }
        ),
        std::make_tuple<std::string, int, std::vector<int> >(
            "192\n283\n654\n987\n%this is the end", // ends on a comment
            1000,
            { 192, 283, 654, 987 }
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

TEST(ParserPatternVector, Errors) {
    test_error("%%MatrixMarket vector coordinate pattern general\n1 1\n ", "expected one field");
    test_error("%%MatrixMarket vector coordinate pattern general\n1 1\n1 1\n", "expected newline");

    test_error("%%MatrixMarket vector coordinate pattern general\n2 1\n1\n2", "more lines present");
    test_error("%%MatrixMarket vector coordinate pattern general\n1 1\n0", "row index must be positive");
    test_error("%%MatrixMarket vector coordinate pattern general\n1 1\n2", "row index out of range");

    test_error("%%MatrixMarket vector coordinate pattern general\n2 2\n1\n", "fewer lines present");

    {
        std::string input = "%%MatrixMarket vector coordinate pattern general\n1 1\n1";
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
        std::string input = "%%MatrixMarket vector array pattern general\n1\n1";
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

TEST(ParserPatternVector, QuitEarly) {
    std::string input = "%%MatrixMarket vector coordinate pattern general\n10 3\n1\n4\n7\n";

    { // quits immediately.
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
        parser.scan_preamble();

        std::vector<int> observed_r;
        EXPECT_FALSE(parser.scan_pattern([&](size_t r, size_t, bool) -> bool {
            observed_r.push_back(r);
            return false;
        }));
        std::vector<int> expected_r{ 1 };
        EXPECT_EQ(observed_r, expected_r);
    }

    { // never quits but function still returns a value.
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
        parser.scan_preamble();

        std::vector<int> observed_r;
        EXPECT_TRUE(parser.scan_pattern([&](size_t r, size_t, bool) -> bool {
            observed_r.push_back(r);
            return true;
        }));
        std::vector<int> expected_r{ 1, 4, 7 };
        EXPECT_EQ(observed_r, expected_r);
    }
}

TEST(ParserPatternVector, Empty) {
    std::string input = "%%MatrixMarket vector coordinate pattern general\n0 0";
    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));

    parser.scan_preamble();

    const auto& deets = parser.get_banner();
    EXPECT_EQ(deets.object, eminem::Object::VECTOR);
    EXPECT_EQ(deets.format, eminem::Format::COORDINATE);
    EXPECT_EQ(deets.field, eminem::Field::PATTERN);
    EXPECT_EQ(deets.symmetry, eminem::Symmetry::GENERAL);

    EXPECT_EQ(parser.get_nrows(), 0);
    EXPECT_EQ(parser.get_ncols(), 1);
    EXPECT_EQ(parser.get_nlines(), 0);

    std::vector<char> observed;
    EXPECT_TRUE(parser.scan_pattern([&](size_t, size_t, bool val) -> void {
        observed.push_back(val);
    }));
    EXPECT_TRUE(observed.empty());
}

TEST(ParserPatternVector, Simulated) {
    size_t N = 4896;
    auto coords = simulate_coordinate(N, 0.05);

    std::stringstream stored;
    format_coordinate(stored, N, coords, std::vector<char>());
    std::string input = stored.str();

    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
    parser.scan_preamble();

    EXPECT_EQ(parser.get_nrows(), N);
    EXPECT_EQ(parser.get_ncols(), 1);
    EXPECT_EQ(parser.get_nlines(), coords.size());

    std::vector<int> out_rows, out_cols;
    std::vector<char> out_vals;
    bool success = parser.scan_pattern([&](size_t r, size_t c, bool v) -> void {
        out_rows.push_back(r - 1);
        out_cols.push_back(c - 1);
        out_vals.push_back(v);
    });

    EXPECT_TRUE(success);

    EXPECT_EQ(out_rows, coords);
    EXPECT_EQ(out_cols, std::vector<int>(coords.size()));
    EXPECT_EQ(out_vals, std::vector<char>(coords.size(), 1));
}

