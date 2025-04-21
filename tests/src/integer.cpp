#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "byteme/byteme.hpp"
#include "eminem/Parser.hpp"

#include <string>
#include <memory>
#include <cstdint>

#include "simulate.h"
#include "format.h"

class ParserIntegerTest : public ::testing::TestWithParam<std::tuple<std::string, int, int, std::vector<int> > > {};

TEST_P(ParserIntegerTest, Success) {
    auto param = GetParam();
    const std::string& content = std::get<0>(param);
    int nr = std::get<1>(param);
    int nc = std::get<2>(param);
    const auto& expected = std::get<3>(param);

    std::string input = "%%MatrixMarket matrix coordinate integer general\n" + std::to_string(nr) + " " + std::to_string(nc) + " " + std::to_string(expected.size()) + "\n" + content;
    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
    parser.scan_preamble();

    const auto& deets = parser.get_banner();
    EXPECT_EQ(deets.object, eminem::Object::MATRIX);
    EXPECT_EQ(deets.format, eminem::Format::COORDINATE);
    EXPECT_EQ(deets.field, eminem::Field::INTEGER);
    EXPECT_EQ(deets.symmetry, eminem::Symmetry::GENERAL);

    EXPECT_EQ(parser.get_nrows(), nr);
    EXPECT_EQ(parser.get_ncols(), nc);
    EXPECT_EQ(parser.get_nlines(), expected.size());

    std::vector<int> observed;
    EXPECT_TRUE(parser.scan_integer([&](eminem::Index, eminem::Index, int val) {
        observed.push_back(val);
    }));
    EXPECT_EQ(observed, expected);
}

INSTANTIATE_TEST_SUITE_P(
    ParserInteger,
    ParserIntegerTest,
    ::testing::Values(
        std::make_tuple<std::string, int, int, std::vector<int> >("1 1 1\n2 2 23\n3 3 456\n4 4 7890\n", 5, 5, { 1, 23, 456, 7890 }), // trailing newline
        std::make_tuple<std::string, int, int, std::vector<int> >("1 1 -1\n2 2 -23\n3 3 -456\n4 4 -7890\n", 5, 5, { -1, -23, -456, -7890 }), // negative values
        std::make_tuple<std::string, int, int, std::vector<int> >("12 34 567890", 50, 50, { 567890 }), // no trailing newline
        std::make_tuple<std::string, int, int, std::vector<int> >("12 34 567890   ", 50, 50, { 567890 }), // trailing blank without a trailing newline.
        std::make_tuple<std::string, int, int, std::vector<int> >("1 1  11  \n2 22   2222\n33 3\t33\t\n44 44 \t4\t \n", 100, 100, { 11, 2222, 33, 4 }) // variable numbers of blanks 
    )
);

static void test_error(const std::string& input, std::string msg) {
    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
    parser.scan_preamble();
    EXPECT_ANY_THROW({
        try {
            parser.scan_integer([&](eminem::Index, eminem::Index, int){});
        } catch (std::exception& e) {
            EXPECT_THAT(e.what(), ::testing::HasSubstr(msg));
            throw;
        }
    });
}

TEST(ParserInteger, Error) {
    test_error("%%MatrixMarket matrix coordinate integer general\n1 1 1\n1 1 -", "premature termination");
    test_error("%%MatrixMarket matrix coordinate integer general\n1 1 1\n1 1 aaron", "expected an integer");
    test_error("%%MatrixMarket matrix coordinate integer general\n1 1 1\n1 1 1 4", "more fields");
    test_error("%%MatrixMarket matrix coordinate integer general\n1 1 1\n1 1 \n", "empty integer field");
    test_error("%%MatrixMarket matrix coordinate integer general\n1 1 1\n1 1\t\n", "empty integer field");

    test_error("%%MatrixMarket vector array integer\n1\n \n", "empty integer field");

    {
        std::string input = "%%MatrixMarket matrix coordinate integer general\n1 1 1\n1 1 1";
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
        EXPECT_ANY_THROW({
            try {
                parser.scan_integer([&](eminem::Index, eminem::Index, int){});
            } catch (std::exception& e) {
                EXPECT_THAT(e.what(), ::testing::HasSubstr("banner or size lines have not yet been parsed"));
                throw;
            }
        });
    }
}

TEST(ParserInteger, OtherType) {
    std::string input = "%%MatrixMarket matrix coordinate integer general\n10 10 3\n1 2 33\n4 5 666\n7 8 9\n";
    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
    parser.scan_preamble();

    std::vector<uint16_t> observed;
    EXPECT_TRUE(parser.template scan_integer<uint16_t>([&](eminem::Index, eminem::Index, uint16_t val) {
        observed.push_back(val);
    }));
    std::vector<uint16_t> expected { 33, 666, 9 };
    EXPECT_EQ(observed, expected);
}

TEST(ParserInteger, QuitEarly) {
    std::string input = "%%MatrixMarket matrix coordinate integer general\n10 10 3\n1 2 33\n4 5 666\n7 8 9\n";
    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
    parser.scan_preamble();

    std::vector<int> observed;
    EXPECT_FALSE(parser.scan_integer([&](eminem::Index, eminem::Index, int val) -> bool {
        observed.push_back(val);
        return val < 100;
    }));
    std::vector<int> expected { 33, 666 };
    EXPECT_EQ(observed, expected);
}

class ParserIntegerSimulatedTest : public ::testing::TestWithParam<std::tuple<int, int> > {
protected:
    eminem::ParserOptions parse_opt;

    void SetUp() {
        auto params = GetParam();
        parse_opt.num_threads = std::get<0>(params);
        parse_opt.block_size = std::get<1>(params);
    }
};

TEST_P(ParserIntegerSimulatedTest, CoordinateMatrix) {
    std::size_t NR = 82, NC = 32;
    auto coords = simulate_coordinate(NR, NC, 0.1);
    auto values = simulate_integer(coords.first.size(), -999, 999);

    std::stringstream stored;
    format_coordinate(stored, NR, NC, coords.first, coords.second, values);
    std::string input = stored.str();

    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)), parse_opt);
    parser.scan_preamble();

    EXPECT_EQ(parser.get_nrows(), NR);
    EXPECT_EQ(parser.get_ncols(), NC);
    EXPECT_EQ(parser.get_nlines(), values.size());

    std::vector<int> out_rows, out_cols, out_vals;
    bool success = parser.scan_integer([&](eminem::Index r, eminem::Index c, int v) -> void {
        out_rows.push_back(r - 1);
        out_cols.push_back(c - 1);
        out_vals.push_back(v);
    });

    EXPECT_TRUE(success);
    EXPECT_EQ(out_rows, coords.first);
    EXPECT_EQ(out_cols, coords.second);
    EXPECT_EQ(out_vals, values);
}

TEST_P(ParserIntegerSimulatedTest, CoordinateVector) {
    std::size_t N = 1392;
    auto coords = simulate_coordinate(N, 0.2);
    auto values = simulate_integer(coords.size(), -999, 999);

    std::stringstream stored;
    format_coordinate(stored, N, coords, values);
    std::string input = stored.str();

    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)), parse_opt);
    parser.scan_preamble();

    EXPECT_EQ(parser.get_nrows(), N);
    EXPECT_EQ(parser.get_nlines(), values.size());

    std::vector<int> out_rows, out_cols, out_vals;
    bool success = parser.scan_integer([&](eminem::Index r, eminem::Index c, int v) -> void {
        out_rows.push_back(r - 1);
        out_cols.push_back(c - 1);
        out_vals.push_back(v);
    });

    EXPECT_TRUE(success);
    EXPECT_EQ(out_rows, coords);
    EXPECT_EQ(out_cols, std::vector<int>(coords.size()));
    EXPECT_EQ(out_vals, values);
}

TEST_P(ParserIntegerSimulatedTest, ArrayMatrix) {
    std::size_t NR = 53, NC = 42;
    auto values = simulate_integer(NR * NC, -999, 999);

    std::stringstream stored;
    format_array(stored, NR, NC, values);
    std::string input = stored.str();

    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)), parse_opt);
    parser.scan_preamble();

    EXPECT_EQ(parser.get_nrows(), NR);
    EXPECT_EQ(parser.get_ncols(), NC);
    EXPECT_EQ(parser.get_nlines(), NR * NC);

    std::vector<int> out_rows, out_cols, out_vals;
    bool success = parser.scan_integer([&](eminem::Index r, eminem::Index c, int v) -> void {
        out_rows.push_back(r - 1);
        out_cols.push_back(c - 1);
        out_vals.push_back(v);
    });

    std::vector<int> expected_rows, expected_cols;
    for (std::size_t c = 0; c < NC; ++c) {
        for (std::size_t r = 0; r < NR; ++r) {
            expected_rows.push_back(r);
            expected_cols.push_back(c);
        }
    }

    EXPECT_TRUE(success);
    EXPECT_EQ(out_rows, expected_rows);
    EXPECT_EQ(out_cols, expected_cols);
    EXPECT_EQ(out_vals, values);
}

TEST_P(ParserIntegerSimulatedTest, ArrayVector) {
    std::size_t N = 1442;
    auto values = simulate_integer(N, -999, 999);

    std::stringstream stored;
    format_array(stored, N, values);
    std::string input = stored.str();

    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)), parse_opt);
    parser.scan_preamble();

    EXPECT_EQ(parser.get_nrows(), N);
    EXPECT_EQ(parser.get_ncols(), 1);
    EXPECT_EQ(parser.get_nlines(), N);

    std::vector<int> out_rows, out_cols, out_vals;
    bool success = parser.scan_integer([&](eminem::Index r, eminem::Index c, int v) -> void {
        out_rows.push_back(r - 1);
        out_cols.push_back(c - 1);
        out_vals.push_back(v);
    });

    EXPECT_TRUE(success);

    std::vector<int> expected_rows(N);
    std::iota(expected_rows.begin(), expected_rows.end(), 0);
    EXPECT_EQ(out_rows, expected_rows);
    EXPECT_EQ(out_cols, std::vector<int>(N));
    EXPECT_EQ(out_vals, values);
}

INSTANTIATE_TEST_SUITE_P(
    ParserInteger,
    ParserIntegerSimulatedTest,
    ::testing::Values(
        std::tuple<int, int>(1, 1),
        std::tuple<int, int>(2, 100),
        std::tuple<int, int>(3, 100),
        std::tuple<int, int>(3, 1000)
    )
);
