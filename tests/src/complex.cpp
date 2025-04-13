#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "byteme/byteme.hpp"
#include "eminem/Parser.hpp"

#include <string>
#include <memory>
#include <complex>

class ParserComplexTest : public ::testing::TestWithParam<std::tuple<std::string, int, int, std::vector<std::complex<double> > > > {};

TEST_P(ParserComplexTest, Success) {
    auto param = GetParam();
    const std::string& content = std::get<0>(param);
    int nr = std::get<1>(param);
    int nc = std::get<2>(param);
    const auto& expected = std::get<3>(param);

    std::string input = "%%MatrixMarket matrix coordinate complex general\n" + std::to_string(nr) + " " + std::to_string(nc) + " " + std::to_string(expected.size()) + "\n" + content;
    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
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
    EXPECT_TRUE(parser.scan_complex([&](size_t, size_t, std::complex<double> val) {
        observed.push_back(val);
    }));
    ASSERT_EQ(observed.size(), expected.size());
    for (size_t i = 0; i < observed.size(); ++i) {
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
            "12 34 567 890", // no trailing newline
            50,
            50,
            { { 567, 890 } }
        ),
        std::make_tuple<std::string, int, int, std::vector<std::complex<double> > >(
            "12 34 567 890   ", // trailing blank without a trailing newline
            50,
            50,
            { { 567, 890 } }
        ),
        std::make_tuple<std::string, int, int, std::vector<std::complex<double> > >(
            "1 1  1.1   0.101 \n2 22\t 22.2  22.22\n33 3\t3.3e3 \t  3.33\t\n44 44 400  \t0.4\t \n", // variable numbers of blanks 
            100,
            100,
            { { 1.1, 0.101 }, { 22.2, 22.22 }, { 3.3e3, 3.33 }, { 400, 0.4 } }
        )
    )
);

static void test_error(const std::string& input, std::string msg) {
    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
    parser.scan_preamble();
    EXPECT_ANY_THROW({
        try {
            parser.scan_complex([&](size_t, size_t, std::complex<double>){});
        } catch (std::exception& e) {
            EXPECT_THAT(e.what(), ::testing::HasSubstr(msg));
            throw;
        }
    });
}

TEST(ParserComplex, Error) {
    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 \n", "empty real"); 
    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 1", "missing the imaginary"); 
    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 1\n", "missing the imaginary"); 
    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 1 ", "missing the imaginary"); 
    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 1 \n", "missing the imaginary"); 

    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 aaron lun", "failed to convert");
    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 1\r 2", "failed to convert"); // trailing whitespace that's not a space, newline or tab.

    test_error("%%MatrixMarket matrix coordinate complex general\n1 1 1\n1 1 1 4 5", "more fields");

    test_error("%%MatrixMarket vector array complex\n1\n \n", "empty real");
    test_error("%%MatrixMarket vector array complex\n1\n1 \n", "missing the imaginary");
}

TEST(ParserComplex, OtherTypes) {
    std::string input = "%%MatrixMarket matrix coordinate complex general\n10 10 3\n1 2 33 3e3\n4 5 66e1 -66\n7 8 -9999 0\n";

    // Single-precision.
    {
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
        parser.scan_preamble();

        std::vector<std::complex<float> > observed;
        EXPECT_TRUE(parser.template scan_complex<float>([&](size_t, size_t, std::complex<float> val) {
            observed.push_back(val);
        }));
        std::vector<std::complex<float> > expected { { 33, 3e3 }, { 660, -66 }, { -9999, 0 } };
        EXPECT_EQ(observed, expected);
    }

    // Super long-precision.
    {
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
        parser.scan_preamble();

        std::vector<std::complex<long double> > observed;
        EXPECT_TRUE(parser.template scan_complex<long double>([&](size_t, size_t, std::complex<long double> val) {
            observed.push_back(val);
        }));
        std::vector<std::complex<long double> > expected { { 33, 3e3 }, { 660, -66 }, { -9999, 0 } };
        EXPECT_EQ(observed, expected);
    }
}

TEST(ParserComplex, QuitEarly) {
    std::string input = "%%MatrixMarket matrix coordinate complex general\n10 10 3\n1 2 300 3E3\n4 5 666 -666\n7 8 0 0\n";
    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
    parser.scan_preamble();

    std::vector<std::complex<double> > observed;
    EXPECT_FALSE(parser.scan_complex([&](size_t, size_t, std::complex<double> val) -> bool {
        observed.push_back(val);
        return val.real() > 0 && val.imag() > 0;
    }));
    std::vector<std::complex<double> > expected { { 300, 3000 }, { 666, -666 } };
    EXPECT_EQ(observed, expected);
}
