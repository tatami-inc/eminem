#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "byteme/byteme.hpp"
#include "eminem/Parser.hpp"

#include <string>
#include <memory>
#include <cstdint>
#include <limits>

class ParserCoordinateVectorTest : public ::testing::TestWithParam<std::tuple<std::string, int, std::vector<int>, std::vector<int> > > {};

TEST_P(ParserCoordinateVectorTest, Success) {
    auto param = GetParam();
    const std::string& content = std::get<0>(param);
    int nr = std::get<1>(param);
    const auto& expected_r = std::get<2>(param);
    const auto& expected_v= std::get<3>(param);

    std::string input = "%%MatrixMarket vector coordinate integer general\n" + std::to_string(nr) + " " + std::to_string(expected_v.size()) + "\n" + content;
    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
    parser.scan_preamble();

    const auto& deets = parser.get_banner();
    EXPECT_EQ(deets.object, eminem::Object::VECTOR);
    EXPECT_EQ(deets.format, eminem::Format::COORDINATE);
    EXPECT_EQ(deets.field, eminem::Field::INTEGER);
    EXPECT_EQ(deets.symmetry, eminem::Symmetry::GENERAL);

    EXPECT_EQ(parser.get_nrows(), nr);
    EXPECT_EQ(parser.get_ncols(), 1);
    EXPECT_EQ(parser.get_nlines(), expected_v.size());

    std::vector<int> observed_r, observed_v;
    EXPECT_TRUE(parser.scan_integer([&](size_t r, size_t c, int val) {
        observed_r.push_back(r);
        EXPECT_EQ(c, 1);
        observed_v.push_back(val);
    }));
    EXPECT_EQ(observed_r, expected_r);
    EXPECT_EQ(observed_v, expected_v);
}

INSTANTIATE_TEST_SUITE_P(
    ParserCoordinateVector,
    ParserCoordinateVectorTest,
    ::testing::Values(
        std::make_tuple<std::string, int, std::vector<int>, std::vector<int> >(
            "51 1\n22 23\n366 456\n479 7890\n", // trailing newline
            500,
            { 51, 22, 366, 479 },
            { 1, 23, 456, 7890 }
        ),
        std::make_tuple<std::string, int, std::vector<int>, std::vector<int> >(
            "119 10\n291 230\n393 4560\n494 789", // no trailing newline
            500,
            { 119, 291, 393, 494 },
            { 10, 230, 4560, 789 }
        ),
        std::make_tuple<std::string, int, std::vector<int>, std::vector<int> >(
            "191 7\n422 8\n73 9\n484 10  ", // trailing blank but no trailing newline
            500,
            { 191, 422, 73, 484 },
            { 7, 8, 9, 10 }
        ),
        std::make_tuple<std::string, int, std::vector<int>, std::vector<int> >(
            "   1 12   \n\t22 \t   345\t\n313\t\t     67  \n444  890\n", // variable numbers of blanks
            500,
            { 1, 22, 313, 444 },
            { 12, 345, 67, 890 }
        ),
        std::make_tuple<std::string, int, std::vector<int>, std::vector<int> >(
            "\n\n19 9\n%iamacomment\n28 13\n%another comment\n\n65 43\n98 5\n", // comments, newlines and crap. 
            100,
            { 19, 28, 65, 98 },
            { 9, 13, 43, 5 }
        ),
        std::make_tuple<std::string, int, std::vector<int>, std::vector<int> >(
            "19 29\n3 13\n44 43\n7 5\n%this is the end", // ends on a comment
            50,
            { 19, 3, 44, 7 },
            { 29, 13, 43, 5 }
        )
    )
);

static void test_error(const std::string& input, std::string msg) {
    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
    parser.scan_preamble();
    EXPECT_ANY_THROW({
        try {
            parser.scan_integer([&](size_t, size_t, int){});
        } catch (std::exception& e) {
            EXPECT_THAT(e.what(), ::testing::HasSubstr(msg));
            throw;
        }
    });
}

TEST(ParserCoordinateVector, Errors) {
    test_error("%%MatrixMarket vector coordinate integer general\n1 1\n ", "expected at least two fields");
    test_error("%%MatrixMarket vector coordinate integer general\n1 1\n1", "unexpected end of file");
    test_error("%%MatrixMarket vector coordinate integer general\n1 1\n1 ", "unexpected end of file");
    test_error("%%MatrixMarket vector coordinate integer general\n1 1\n1\n", "unexpected newline");
    test_error("%%MatrixMarket vector coordinate integer general\n1 1\n1 \n", "empty integer field");

    test_error("%%MatrixMarket vector coordinate integer general\n2 1\n1 1\n2 2", "more lines present");
    test_error("%%MatrixMarket vector coordinate integer general\n1 1\n0 1", "row index must be positive");
    test_error("%%MatrixMarket vector coordinate integer general\n1 1\n2 1", "row index out of range");

    test_error("%%MatrixMarket vector coordinate integer general\n2 2\n1 1\n", "fewer lines present");

    {
        std::string input = "%%MatrixMarket vector coordinate integer general\n1 1 1\n1 1 1";
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

TEST(ParserCoordinateVector, Types) {
    { // Checking it works with real.
        std::string input = "%%MatrixMarket vector coordinate real general\n10 3\n1 1.2e-4\n5 -12.34\n2 inf\n";
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
        parser.scan_preamble();

        const auto& deets = parser.get_banner();
        EXPECT_EQ(deets.object, eminem::Object::VECTOR);
        EXPECT_EQ(deets.format, eminem::Format::COORDINATE);
        EXPECT_EQ(deets.field, eminem::Field::REAL);
        EXPECT_EQ(deets.symmetry, eminem::Symmetry::GENERAL);

        EXPECT_EQ(parser.get_nrows(), 10);
        EXPECT_EQ(parser.get_ncols(), 1);
        EXPECT_EQ(parser.get_nlines(), 3);

        std::vector<int> observed_r;
        std::vector<double> observed_v;
        EXPECT_TRUE(parser.scan_real([&](size_t r, size_t c, double val) {
            observed_r.push_back(r);
            EXPECT_EQ(c, 1);
            observed_v.push_back(val);
        }));

        std::vector<int> expected_r { 1, 5, 2 };
        EXPECT_EQ(observed_r, expected_r);
        std::vector<double> expected_v { 1.2e-4, -12.34, std::numeric_limits<double>::infinity() };
        ASSERT_EQ(observed_v.size(), expected_v.size());
        for (size_t i = 0; i < observed_v.size(); ++i) {
            EXPECT_DOUBLE_EQ(observed_v[i], expected_v[i]);
        }
    }

    { // Checking it works with complex.
        std::string input = "%%MatrixMarket vector coordinate complex general\n10 3\n1 78 1.2e-4\n5 12.34 -9.9\n2 inf 10\n";
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
        parser.scan_preamble();

        const auto& deets = parser.get_banner();
        EXPECT_EQ(deets.object, eminem::Object::VECTOR);
        EXPECT_EQ(deets.format, eminem::Format::COORDINATE);
        EXPECT_EQ(deets.field, eminem::Field::COMPLEX);
        EXPECT_EQ(deets.symmetry, eminem::Symmetry::GENERAL);

        EXPECT_EQ(parser.get_nrows(), 10);
        EXPECT_EQ(parser.get_ncols(), 1);
        EXPECT_EQ(parser.get_nlines(), 3);

        std::vector<int> observed_r, observed_c;
        std::vector<std::complex<double> > observed_v;
        EXPECT_TRUE(parser.scan_complex([&](size_t r, size_t c, std::complex<double> val) {
            observed_r.push_back(r);
            EXPECT_EQ(c, 1);
            observed_v.push_back(val);
        }));

        std::vector<int> expected_r { 1, 5, 2 };
        EXPECT_EQ(observed_r, expected_r);
        std::vector<std::complex<double> > expected_v { { 78, 1.2e-4 } , { 12.34, -9.9 }, { std::numeric_limits<double>::infinity(), 10 } };
        ASSERT_EQ(observed_v.size(), expected_v.size());
        for (size_t i = 0; i < observed_v.size(); ++i) {
            EXPECT_DOUBLE_EQ(observed_v[i].real(), expected_v[i].real());
            EXPECT_DOUBLE_EQ(observed_v[i].imag(), expected_v[i].imag());
        }
    }
}

TEST(ParserCoordinateVector, QuitEarly) {
    std::string input = "%%MatrixMarket vector coordinate integer general\n10 3\n1 33\n4 666\n7 9\n";

    { // quits immediately.
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
        parser.scan_preamble();

        std::vector<int> observed;
        EXPECT_FALSE(parser.scan_integer([&](size_t, size_t, int val) -> bool {
            observed.push_back(val);
            return false;
        }));
        std::vector<int> expected { 33 };
        EXPECT_EQ(observed, expected);
    }

    { // never quits but still returns a value.
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
        parser.scan_preamble();

        std::vector<int> observed;
        EXPECT_TRUE(parser.scan_integer([&](size_t, size_t, int val) -> bool {
            observed.push_back(val);
            return true;
        }));
        std::vector<int> expected { 33, 666, 9 };
        EXPECT_EQ(observed, expected);
    }
}

TEST(ParserCoordinateVector, Empty) {
    std::string input = "%%MatrixMarket vector coordinate integer general\n0 0";
    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));

    parser.scan_preamble();

    const auto& deets = parser.get_banner();
    EXPECT_EQ(deets.object, eminem::Object::VECTOR);
    EXPECT_EQ(deets.format, eminem::Format::COORDINATE);
    EXPECT_EQ(deets.field, eminem::Field::INTEGER);
    EXPECT_EQ(deets.symmetry, eminem::Symmetry::GENERAL);

    EXPECT_EQ(parser.get_nrows(), 0);
    EXPECT_EQ(parser.get_ncols(), 1);
    EXPECT_EQ(parser.get_nlines(), 0);

    std::vector<int> observed;
    EXPECT_TRUE(parser.scan_integer([&](size_t, size_t, int val) {
        observed.push_back(val);
    }));
    EXPECT_TRUE(observed.empty());
}
