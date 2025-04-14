#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "byteme/byteme.hpp"
#include "eminem/Parser.hpp"

#include <string>
#include <memory>

#include "simulate.h"
#include "format.h"

class ParserRealTest : public ::testing::TestWithParam<std::tuple<std::string, int, int, std::vector<double> > > {};

TEST_P(ParserRealTest, Success) {
    auto param = GetParam();
    const std::string& content = std::get<0>(param);
    int nr = std::get<1>(param);
    int nc = std::get<2>(param);
    const auto& expected = std::get<3>(param);

    std::string input = "%%MatrixMarket matrix coordinate real general\n" + std::to_string(nr) + " " + std::to_string(nc) + " " + std::to_string(expected.size()) + "\n" + content;
    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
    parser.scan_preamble();

    const auto& deets = parser.get_banner();
    EXPECT_EQ(deets.object, eminem::Object::MATRIX);
    EXPECT_EQ(deets.format, eminem::Format::COORDINATE);
    EXPECT_EQ(deets.field, eminem::Field::REAL);
    EXPECT_EQ(deets.symmetry, eminem::Symmetry::GENERAL);

    EXPECT_EQ(parser.get_nrows(), nr);
    EXPECT_EQ(parser.get_ncols(), nc);
    EXPECT_EQ(parser.get_nlines(), expected.size());

    std::vector<double> observed;
    EXPECT_TRUE(parser.scan_real([&](size_t, size_t, double val) {
        observed.push_back(val);
    }));
    ASSERT_EQ(observed.size(), expected.size());
    for (size_t i = 0; i < observed.size(); ++i) {
        EXPECT_DOUBLE_EQ(observed[i], expected[i]);
    }
}

INSTANTIATE_TEST_SUITE_P(
    ParserReal,
    ParserRealTest,
    ::testing::Values(
        std::make_tuple<std::string, int, int, std::vector<double> >("1 1 1.2\n2 2 2e3\n3 3 -456\n4 4 0.789\n", 5, 5, { 1.2, 2e3, -456, 0.789 }), // trailing newline
        std::make_tuple<std::string, int, int, std::vector<double> >("12 34 567.890", 50, 50, { 567.890 }), // no trailing newline
        std::make_tuple<std::string, int, int, std::vector<double> >("12 34 567.890   ", 50, 50, { 567.890 }), // trailing blank without a trailing newline.
        std::make_tuple<std::string, int, int, std::vector<double> >("1 1  1.1  \n2 22   22.22\n33 3\t3.3e3\t\n44 44 \t0.4\t \n", 100, 100, { 1.1, 22.22, 3.3e3, 0.4 }) // variable numbers of blanks 
    )
);

static void test_error(const std::string& input, std::string msg) {
    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
    parser.scan_preamble();
    EXPECT_ANY_THROW({
        try {
            parser.scan_real([&](size_t, size_t, double){});
        } catch (std::exception& e) {
            EXPECT_THAT(e.what(), ::testing::HasSubstr(msg));
            throw;
        }
    });
}

TEST(ParserReal, Error) {
    test_error("%%MatrixMarket matrix coordinate real general\n1 1 1\n1 1 aaron", "failed to convert");
    test_error("%%MatrixMarket matrix coordinate real general\n1 1 1\n1 1 1\r", "failed to convert"); // trailing whitespace that's not a space, newline or tab.
    test_error("%%MatrixMarket matrix coordinate real general\n1 1 1\n1 1 1 4", "more fields");
    test_error("%%MatrixMarket matrix coordinate real general\n1 1 1\n1 1 \n", "empty number field");
    test_error("%%MatrixMarket matrix coordinate real general\n1 1 1\n1 1\t\n", "empty number field");

    test_error("%%MatrixMarket vector array real\n1\n \n", "empty number field");

    {
        std::string input = "%%MatrixMarket vector coordinate real general\n1 1 1\n1 1 1";
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
        EXPECT_ANY_THROW({
            try {
                parser.scan_integer([&](size_t, size_t, int){});
            } catch (std::exception& e) {
                EXPECT_THAT(e.what(), ::testing::HasSubstr("banner or size lines have not yet been parsed"));
                throw;
            }
        });
    }
}

TEST(ParserReal, OtherTypes) {
    std::string input = "%%MatrixMarket matrix coordinate real general\n10 10 3\n1 2 33\n4 5 666\n7 8 9\n";

    // Single-precision.
    {
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
        parser.scan_preamble();

        std::vector<float> observed;
        EXPECT_TRUE(parser.template scan_real<float>([&](size_t, size_t, float val) {
            observed.push_back(val);
        }));
        std::vector<float> expected { 33, 666, 9 };
        EXPECT_EQ(observed, expected);
    }

    // Super long-precision.
    {
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
        parser.scan_preamble();

        std::vector<long double> observed;
        EXPECT_TRUE(parser.template scan_real<long double>([&](size_t, size_t, long double val) {
            observed.push_back(val);
        }));
        std::vector<long double> expected { 33, 666, 9 };
        EXPECT_EQ(observed, expected);
    }
}

TEST(ParserReal, Specials) {
    std::string input = "%%MatrixMarket matrix coordinate real general\n10 10 4\n1 2 inf\n4 5 -INF\n7 8 nan\n9 10 -NaN\n";

    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
    parser.scan_preamble();

    std::vector<double> observed;
    EXPECT_TRUE(parser.scan_double([&](size_t, size_t, double val) { // using scan_double() to get some coverage of the alias.
        observed.push_back(val);
    }));
    ASSERT_EQ(observed.size(), 4);
    EXPECT_EQ(observed[0], std::numeric_limits<double>::infinity());
    EXPECT_EQ(observed[1], -std::numeric_limits<double>::infinity());
    EXPECT_TRUE(std::isnan(observed[2]));
    EXPECT_TRUE(std::isnan(observed[3]));
}

TEST(ParserReal, QuitEarly) {
    std::string input = "%%MatrixMarket matrix coordinate real general\n10 10 3\n1 2 33\n4 5 666\n7 8 9\n";
    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
    parser.scan_preamble();

    std::vector<double> observed;
    EXPECT_FALSE(parser.scan_real([&](size_t, size_t, double val) -> bool {
        observed.push_back(val);
        return val < 100;
    }));
    std::vector<double> expected { 33, 666 };
    EXPECT_EQ(observed, expected);
}

static void test_equal_vectors(const std::vector<double>& left, const std::vector<double>& right) {
    // using a more generous tolerance to account for loss of precision when saving to text.
    ASSERT_EQ(left.size(), right.size());
    for (size_t i = 0; i < left.size(); ++i) {
        EXPECT_TRUE(std::abs(left[i] - right[i]) <= 0.00000001);
    }
}

TEST(ParserReal, SimulatedCoordinateMatrix) {
    size_t NR = 65, NC = 58;
    auto coords = simulate_coordinate(NR, NC, 0.1);
    auto values = simulate_real(coords.first.size());

    std::stringstream stored;
    format_coordinate(stored, NR, NC, coords.first, coords.second, values);
    std::string input = stored.str();

    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
    parser.scan_preamble();

    EXPECT_EQ(parser.get_nrows(), NR);
    EXPECT_EQ(parser.get_ncols(), NC);
    EXPECT_EQ(parser.get_nlines(), values.size());

    std::vector<int> out_rows, out_cols;
    std::vector<double> out_vals;
    bool success = parser.scan_real([&](size_t r, size_t c, double v) -> void {
        out_rows.push_back(r - 1);
        out_cols.push_back(c - 1);
        out_vals.push_back(v);
    });

    EXPECT_TRUE(success);
    EXPECT_EQ(out_rows, coords.first);
    EXPECT_EQ(out_cols, coords.second);
    test_equal_vectors(out_vals, values);
}

TEST(ParserReal, SimulatedCoordinateVector) {
    size_t N = 6558;
    auto coords = simulate_coordinate(N, 0.05);
    auto values = simulate_real(coords.size());

    std::stringstream stored;
    format_coordinate(stored, N, coords, values);
    std::string input = stored.str();

    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
    parser.scan_preamble();

    EXPECT_EQ(parser.get_nrows(), N);
    EXPECT_EQ(parser.get_ncols(), 1);
    EXPECT_EQ(parser.get_nlines(), values.size());

    std::vector<int> out_rows, out_cols;
    std::vector<double> out_vals;
    bool success = parser.scan_real([&](size_t r, size_t c, double v) -> void {
        out_rows.push_back(r - 1);
        out_cols.push_back(c - 1);
        out_vals.push_back(v);
    });

    EXPECT_TRUE(success);
    EXPECT_EQ(out_rows, coords);
    EXPECT_EQ(out_cols, std::vector<int>(coords.size()));
    test_equal_vectors(out_vals, values);
}

TEST(ParserReal, SimulatedArrayMatrix) {
    size_t NR = 93, NC = 85;
    auto values = simulate_real(NR * NC);

    std::stringstream stored;
    format_array(stored, NR, NC, values);
    std::string input = stored.str();

    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
    parser.scan_preamble();

    EXPECT_EQ(parser.get_nrows(), NR);
    EXPECT_EQ(parser.get_ncols(), NC);
    EXPECT_EQ(parser.get_nlines(), NR * NC);

    std::vector<int> out_rows, out_cols;
    std::vector<double> out_vals;
    bool success = parser.scan_real([&](size_t r, size_t c, double v) -> void {
        out_rows.push_back(r - 1);
        out_cols.push_back(c - 1);
        out_vals.push_back(v);
    });

    EXPECT_TRUE(success);

    std::vector<int> expected_rows, expected_cols;
    for (size_t c = 0; c < NC; ++c) {
        for (size_t r = 0; r < NR; ++r) {
            expected_rows.push_back(r);
            expected_cols.push_back(c);
        }
    }

    EXPECT_EQ(out_rows, expected_rows);
    EXPECT_EQ(out_cols, expected_cols);
    test_equal_vectors(out_vals, values);
}

TEST(ParserReal, SimulatedArrayVector) {
    size_t N = 632;
    auto values = simulate_real(N);

    std::stringstream stored;
    format_array(stored, N, values);
    std::string input = stored.str();

    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
    parser.scan_preamble();

    EXPECT_EQ(parser.get_nrows(), N);
    EXPECT_EQ(parser.get_ncols(), 1);
    EXPECT_EQ(parser.get_nlines(), N);

    std::vector<int> out_rows, out_cols;
    std::vector<double> out_vals;
    bool success = parser.scan_real([&](size_t r, size_t c, double v) -> void {
        out_rows.push_back(r - 1);
        out_cols.push_back(c - 1);
        out_vals.push_back(v);
    });

    EXPECT_TRUE(success);

    std::vector<int> expected_rows(N);
    std::iota(expected_rows.begin(), expected_rows.end(), 0);
    EXPECT_EQ(out_rows, expected_rows);
    EXPECT_EQ(out_cols, std::vector<int>(N));
    test_equal_vectors(out_vals, values);
}
