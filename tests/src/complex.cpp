#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "byteme/byteme.hpp"
#include "eminem/Parser.hpp"

#include <string>
#include <memory>
#include <complex>

#include "simulate.h"
#include "format.h"

class ParserComplexTest : public ::testing::TestWithParam<std::tuple<std::string, int, int, std::vector<std::complex<double> > > > {};

TEST_P(ParserComplexTest, Success) {
    auto param = GetParam();
    const std::string& content = std::get<0>(param);
    int nr = std::get<1>(param);
    int nc = std::get<2>(param);
    const auto& expected = std::get<3>(param);

    std::string input = "%%MatrixMarket matrix coordinate complex general\n" + std::to_string(nr) + " " + std::to_string(nc) + " " + std::to_string(expected.size()) + "\n" + content;
    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)), {});
    parser.scan_preamble();

    const auto& deets = parser.get_banner();
    EXPECT_EQ(deets.object, eminem::Object::MATRIX);
    EXPECT_EQ(deets.format, eminem::Format::COORDINATE);
    EXPECT_EQ(deets.field, eminem::Field::COMPLEX);
    EXPECT_EQ(deets.symmetry, eminem::Symmetry::GENERAL);

    EXPECT_EQ(parser.get_nrows(), nr);
    EXPECT_EQ(parser.get_ncols(), nc);
    EXPECT_EQ(parser.get_nlines(), expected.size());

    std::vector<std::complex<double> > observed;
    EXPECT_TRUE(parser.scan_complex([&](eminem::Index, eminem::Index, std::complex<double> val) {
        observed.push_back(val);
    }));
    ASSERT_EQ(observed.size(), expected.size());
    for (decltype(observed.size()) i = 0, end = observed.size(); i < end; ++i) {
        EXPECT_DOUBLE_EQ(observed[i].real(), expected[i].real());
        EXPECT_DOUBLE_EQ(observed[i].imag(), expected[i].imag());
    }
}

INSTANTIATE_TEST_SUITE_P(
    ParserComplex,
    ParserComplexTest,
    ::testing::Values(
        std::make_tuple<std::string, int, int, std::vector<std::complex<double> > >(
            "1 1 1.2 2.1\n2 2 2e3 3.3E2\n3 3 -45 -6\n4 4 0.78 9.0\n", // trailing newline
            5,
            5,
            { { 1.2, 2.1 }, { 2e3, 3.3e2 }, { -45, -6 }, { 0.78, 9.0 } }
        ),

        std::make_tuple<std::string, int, int, std::vector<std::complex<double> > >(
            "12 34 567 890\n", // trailing newline after integer
            50,
            50,
            { { 567, 890 } }
        ),
        std::make_tuple<std::string, int, int, std::vector<std::complex<double> > >(
            "12 34 567 890   \n", // trailing blank with a trailing newline after integer
            50,
            50,
            { { 567, 890 } }
        ),
        std::make_tuple<std::string, int, int, std::vector<std::complex<double> > >(
            "12 34 567 890", // no trailing newline after integer
            50,
            50,
            { { 567, 890 } }
        ),
        std::make_tuple<std::string, int, int, std::vector<std::complex<double> > >(
            "12 34 567 890   ", // trailing blank without a trailing newline after integer
            50,
            50,
            { { 567, 890 } }
        ),

        std::make_tuple<std::string, int, int, std::vector<std::complex<double> > >(
            "12 34 5.67 8.90\n", // trailing newline after fraction
            50,
            50,
            { { 5.67, 8.90 } }
        ),
        std::make_tuple<std::string, int, int, std::vector<std::complex<double> > >(
            "12 34 5.67 8.90   \n", // trailing blank with a trailing newline after fraction
            50,
            50,
            { { 5.67, 8.90 } }
        ),
        std::make_tuple<std::string, int, int, std::vector<std::complex<double> > >(
            "12 34 5.67 8.90", // no trailing newline after fraction
            50,
            50,
            { { 5.67, 8.90 } }
        ),
        std::make_tuple<std::string, int, int, std::vector<std::complex<double> > >(
            "12 34 5.67 8.90   ", // trailing blank without a trailing newline after fraction
            50,
            50,
            { { 5.67, 8.90 } }
        ),

        std::make_tuple<std::string, int, int, std::vector<std::complex<double> > >(
            "12 34 5.6e7 89e+0\n", // trailing newline after exponent 
            50,
            50,
            { { 5.6e7, 89 } }
        ),
        std::make_tuple<std::string, int, int, std::vector<std::complex<double> > >(
            "12 34 5.6e7 89e+0   \n", // trailing blank with a trailing newline after exponent
            50,
            50,
            { { 5.6e7, 89 } }
        ),
        std::make_tuple<std::string, int, int, std::vector<std::complex<double> > >(
            "12 34 5.6e7 89e+0", // no trailing newline after exponent
            50,
            50,
            { { 5.6e7, 89 } }
        ),
        std::make_tuple<std::string, int, int, std::vector<std::complex<double> > >(
            "12 34 5.6e7 89e+0   ", // trailing blank without a trailing newline after exponent
            50,
            50,
            { { 5.6e7, 89 } }
        ),

        std::make_tuple<std::string, int, int, std::vector<std::complex<double> > >(
            "1 1  1.1   0.101 \n2 22\t 22.2  22.22\n33 3\t3.3e3 \t  3.33\t\n44 44 400  \t0.4\t \n", // variable numbers of blanks 
            100,
            100,
            { { 1.1, 0.101 }, { 22.2, 22.22 }, { 3.3e3, 3.33 }, { 400, 0.4 } }
        ),
        std::make_tuple<std::string, int, int, std::vector<std::complex<double> > >(
            "1 1 1. -.1\n2 22 22. .222\n", // decimals without any other digits
            100,
            100,
            { { 1.0, -0.1 }, { 22.0, 0.222 } }
        ),
        std::make_tuple<std::string, int, int, std::vector<std::complex<double> > >(
            "1 1 +1.1 -0.101\n2 22 0222 00.2\n33 3 03.3e+3 -003.33\n44 2 44e-002 000.4\n", // signs and leading zeros.
            100,
            100,
            { { 1.1, -0.101 }, { 222.0, 0.2 }, { 3.3e3, -3.33 }, { 0.44, 0.4 } }
        )
    )
);

static void test_error(const std::string& input, std::string msg) {
    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)), {});
    parser.scan_preamble();
    EXPECT_ANY_THROW({
        try {
            parser.scan_complex([&](eminem::Index, eminem::Index, std::complex<double>){});
        } catch (std::exception& e) {
            EXPECT_THAT(e.what(), ::testing::HasSubstr(msg));
            throw;
        }
    });
}

TEST(ParserComplex, Error) {
    // Most of the error conditions are redundant with those in ParseReal, so we'll focus on the last_=false case.
    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 \n", "unexpected newline"); 
    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 1", "unexpected end of file"); 
    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 1\n", "unexpected newline"); 
    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 1 ", "unexpected end of file"); 
    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 1 \n", "no digits"); 
    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 1 4 5", "more fields");
    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 a 4", "unrecognized");

    // Checking the decimal and exponent EOF errors separately.
    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 1.", "unexpected end of file"); 
    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 1.\n", "unexpected newline"); 
    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 1. ", "unexpected end of file"); 
    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 1. \n", "no digits"); 
    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 1.12 4.23 5", "more fields");
    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 1... 4.34", "unrecognized");

    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 1e1", "unexpected end of file"); 
    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 1e1\n", "unexpected newline"); 
    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 1e1 ", "unexpected end of file"); 
    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 1e1 \n", "no digits"); 
    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 1e+ \n", "no digits"); 
    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 1 4e+2 5", "more fields");
    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 4ee2 5e2", "unrecognized");

    test_error("%%MatrixMarket vector array complex\n1\n \n", "unexpected newline");
    test_error("%%MatrixMarket vector array complex\n1\n1 \n", "no digits");

    {
        std::string input = "%%MatrixMarket matrix array complex general\n1 1\n1 1";
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)), {});
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

TEST(ParserComplex, Specials) {
    for (int i = 0; i < 2; ++i) {
        std::string input = "%%MatrixMarket matrix coordinate complex general\n10 10 2\n";
        if (i == 1) {
            input += "1 2 inf -INFINITY\n4 5 nan -NaN\n";
        } else {
            // Sprinkling in some spaces for fun.
            input += "1 2 \t inf   -INFINITY  \n4 5\tnan    -NaN\t\n";
        }

        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)), {});
        parser.scan_preamble();

        std::vector<std::complex<double> > observed;
        EXPECT_TRUE(parser.scan_complex([&](eminem::Index, eminem::Index, std::complex<double> val) { // using scan_double() to get some coverage of the alias.
            observed.push_back(val);
        }));
        ASSERT_EQ(observed.size(), 2);
        EXPECT_EQ(observed[0].real(), std::numeric_limits<double>::infinity());
        EXPECT_EQ(observed[0].imag(), -std::numeric_limits<double>::infinity());
        EXPECT_TRUE(std::isnan(observed[1].real()));
        EXPECT_TRUE(std::isnan(observed[1].imag()));
    }

    // No terminating newlines at EOF.
    for (int i = 0; i < 2; ++i) {
        std::string input = "%%MatrixMarket matrix coordinate real general\n10 10 1\n1 2 NAN INF";
        if (i == 1) {
            input += "   "; // space before EOF.
        }

        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)), {});
        parser.scan_preamble();

        std::vector<std::complex<double> > observed;
        EXPECT_TRUE(parser.scan_complex([&](eminem::Index, eminem::Index, std::complex<double> val) { // using scan_double() to get some coverage of the alias.
            observed.push_back(val);
        }));
        ASSERT_EQ(observed.size(), 1);
        EXPECT_TRUE(std::isnan(observed[0].real()));
        EXPECT_TRUE(std::isinf(observed[0].imag()));
    }

    // Various errors.
    test_error("%%MatrixMarket matrix coordinate real general\n1 1 1\n1 1 in", "unexpected termination");
    test_error("%%MatrixMarket matrix coordinate real general\n1 1 1\n1 1 inf", "unexpected end of file");
    test_error("%%MatrixMarket matrix coordinate real general\n1 1 1\n1 1 infinity", "unexpected end of file");
    test_error("%%MatrixMarket matrix coordinate real general\n1 1 1\n1 1 nan", "unexpected end of file");
    test_error("%%MatrixMarket matrix coordinate real general\n1 1 1\n1 1 inf\t", "unexpected end of file");
    test_error("%%MatrixMarket matrix coordinate real general\n1 1 1\n1 1 inf ", "unexpected end of file");
    test_error("%%MatrixMarket matrix coordinate real general\n1 1 1\n1 1 inf\n", "unexpected newline");
    test_error("%%MatrixMarket matrix coordinate real general\n1 1 1\n1 1 ina", "unexpected character");

    for (int i = 0; i < 2; ++i) {
        std::string input = "%%MatrixMarket matrix coordinate real general\n10 10 1\n1 2 ";
        if (i == 1) {
            input += "NaN";
        } else {
            input += "Inf";
        }
        input += " NAN";

        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)), {});
        parser.scan_preamble();
        EXPECT_ANY_THROW({
            try {
                parser.scan_complex<int>([&](eminem::Index, eminem::Index, std::complex<int>){});
            } catch (std::exception& e) {
                EXPECT_THAT(e.what(), ::testing::HasSubstr("requested type does not support"));
                throw;
            }
        });
    }
}

TEST(ParserComplex, OtherTypes) {
    std::string input = "%%MatrixMarket matrix coordinate complex general\n10 10 3\n1 2 33 3e3\n4 5 66e1 -66\n7 8 -9999 0\n";

    // Single-precision.
    {
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)), {});
        parser.scan_preamble();

        std::vector<std::complex<float> > observed;
        EXPECT_TRUE(parser.template scan_complex<float>([&](eminem::Index, eminem::Index, std::complex<float> val) {
            observed.push_back(val);
        }));
        std::vector<std::complex<float> > expected { { 33, 3e3 }, { 660, -66 }, { -9999, 0 } };
        EXPECT_EQ(observed, expected);
    }

    // Super long-precision.
    {
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)), {});
        parser.scan_preamble();

        std::vector<std::complex<long double> > observed;
        EXPECT_TRUE(parser.template scan_complex<long double>([&](eminem::Index, eminem::Index, std::complex<long double> val) {
            observed.push_back(val);
        }));
        std::vector<std::complex<long double> > expected { { 33, 3e3 }, { 660, -66 }, { -9999, 0 } };
        EXPECT_EQ(observed, expected);
    }
}

TEST(ParserComplex, QuitEarly) {
    std::string input = "%%MatrixMarket matrix coordinate complex general\n10 10 3\n1 2 300 3E3\n4 5 666 -666\n7 8 0 0\n";
    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)), {});
    parser.scan_preamble();

    std::vector<std::complex<double> > observed;
    EXPECT_FALSE(parser.scan_complex([&](eminem::Index, eminem::Index, std::complex<double> val) -> bool {
        observed.push_back(val);
        return val.real() > 0 && val.imag() > 0;
    }));
    std::vector<std::complex<double> > expected { { 300, 3000 }, { 666, -666 } };
    EXPECT_EQ(observed, expected);
}

class ParserComplexSimulatedTest : public ::testing::TestWithParam<std::tuple<int, int> > {
protected:
    eminem::ParserOptions parse_opt;

    void SetUp() {
        auto params = GetParam();
        parse_opt.num_threads = std::get<0>(params);
        parse_opt.block_size = std::get<1>(params);
    }
};

static void test_equal_vectors(const std::vector<std::complex<double> >& observed, const std::vector<std::complex<double> >& expected) {
    ASSERT_EQ(observed.size(), expected.size());
    for (decltype(observed.size()) i = 0, end = observed.size(); i < end; ++i) {
        EXPECT_TRUE(std::abs(observed[i].real() - expected[i].real()) <= 0.00000001);
        EXPECT_TRUE(std::abs(observed[i].imag() - expected[i].imag()) <= 0.00000001);
    }
}

TEST_P(ParserComplexSimulatedTest, CoordinateMatrix) {
    std::size_t NR = 65, NC = 58;
    auto coords = simulate_coordinate(NR, NC, 0.1);
    auto values = simulate_complex(coords.first.size());

    std::stringstream stored;
    format_coordinate(stored, NR, NC, coords.first, coords.second, values);
    std::string input = stored.str();

    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)), parse_opt);
    parser.scan_preamble();

    EXPECT_EQ(parser.get_nrows(), NR);
    EXPECT_EQ(parser.get_ncols(), NC);
    EXPECT_EQ(parser.get_nlines(), values.size());

    std::vector<int> out_rows, out_cols;
    std::vector<std::complex<double> > out_vals;
    bool success = parser.scan_complex([&](eminem::Index r, eminem::Index c, std::complex<double> v) -> void {
        out_rows.push_back(r - 1);
        out_cols.push_back(c - 1);
        out_vals.push_back(v);
    });

    EXPECT_TRUE(success);
    EXPECT_EQ(out_rows, coords.first);
    EXPECT_EQ(out_cols, coords.second);
    test_equal_vectors(out_vals, values);
}

TEST_P(ParserComplexSimulatedTest, CoordinateVector) {
    std::size_t N = 6558;
    auto coords = simulate_coordinate(N, 0.05);
    auto values = simulate_complex(coords.size());

    std::stringstream stored;
    format_coordinate(stored, N, coords, values);
    stored << "\n"; // inject an extra newline.
    std::string input = stored.str();

    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)), parse_opt);
    parser.scan_preamble();

    EXPECT_EQ(parser.get_nrows(), N);
    EXPECT_EQ(parser.get_ncols(), 1);
    EXPECT_EQ(parser.get_nlines(), values.size());

    std::vector<int> out_rows, out_cols;
    std::vector<std::complex<double> > out_vals;
    bool success = parser.scan_complex([&](eminem::Index r, eminem::Index c, std::complex<double> v) -> void {
        out_rows.push_back(r - 1);
        out_cols.push_back(c - 1);
        out_vals.push_back(v);
    });

    EXPECT_TRUE(success);
    EXPECT_EQ(out_rows, coords);
    EXPECT_EQ(out_cols, std::vector<int>(coords.size()));
    test_equal_vectors(out_vals, values);
}

TEST_P(ParserComplexSimulatedTest, ArrayMatrix) {
    std::size_t NR = 93, NC = 85;
    auto values = simulate_complex(NR * NC);

    std::stringstream stored;
    format_array(stored, NR, NC, values);
    std::string input = stored.str();

    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)), parse_opt);
    parser.scan_preamble();

    EXPECT_EQ(parser.get_nrows(), NR);
    EXPECT_EQ(parser.get_ncols(), NC);
    EXPECT_EQ(parser.get_nlines(), NR * NC);

    std::vector<int> out_rows, out_cols;
    std::vector<std::complex<double> > out_vals;
    bool success = parser.scan_complex([&](eminem::Index r, eminem::Index c, std::complex<double> v) -> void {
        out_rows.push_back(r - 1);
        out_cols.push_back(c - 1);
        out_vals.push_back(v);
    });

    EXPECT_TRUE(success);

    std::vector<int> expected_rows, expected_cols;
    for (std::size_t c = 0; c < NC; ++c) {
        for (std::size_t r = 0; r < NR; ++r) {
            expected_rows.push_back(r);
            expected_cols.push_back(c);
        }
    }

    EXPECT_EQ(out_rows, expected_rows);
    EXPECT_EQ(out_cols, expected_cols);
    test_equal_vectors(out_vals, values);
}

TEST_P(ParserComplexSimulatedTest, ArrayVector) {
    std::size_t N = 632;
    auto values = simulate_complex(N);

    std::stringstream stored;
    format_array(stored, N, values);
    std::string input = stored.str();

    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)), parse_opt);
    parser.scan_preamble();

    EXPECT_EQ(parser.get_nrows(), N);
    EXPECT_EQ(parser.get_ncols(), 1);
    EXPECT_EQ(parser.get_nlines(), N);

    std::vector<int> out_rows, out_cols;
    std::vector<std::complex<double> > out_vals;
    bool success = parser.scan_complex([&](eminem::Index r, eminem::Index c, std::complex<double> v) -> void {
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

INSTANTIATE_TEST_SUITE_P(
    ParserComplex,
    ParserComplexSimulatedTest,
    ::testing::Values(
        std::tuple<int, int>(1, 1),
        std::tuple<int, int>(2, 100),
        std::tuple<int, int>(3, 100),
        std::tuple<int, int>(3, 1000)
    )
);
