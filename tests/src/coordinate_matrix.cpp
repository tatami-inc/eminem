#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "byteme/byteme.hpp"
#include "eminem/Parser.hpp"

#include <string>
#include <memory>
#include <cstdint>
#include <limits>

class ParserCoordinateMatrixTest : public ::testing::TestWithParam<std::tuple<std::string, int, int, std::vector<int>, std::vector<int>, std::vector<int> > > {};

TEST_P(ParserCoordinateMatrixTest, Success) {
    auto param = GetParam();
    const std::string& content = std::get<0>(param);
    int nr = std::get<1>(param);
    int nc = std::get<2>(param);
    const auto& expected_r = std::get<3>(param);
    const auto& expected_c = std::get<4>(param);
    const auto& expected_v = std::get<5>(param);

    std::string input = "%%MatrixMarket matrix coordinate integer general\n" + std::to_string(nr) + " " + std::to_string(nc) + " " + std::to_string(expected_v.size()) + "\n" + content;
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
    EXPECT_EQ(parser.get_nlines(), expected_v.size());

    std::vector<int> observed_r, observed_c, observed_v;
    EXPECT_TRUE(parser.scan_integer([&](size_t r, size_t c, int val) {
        observed_r.push_back(r);
        observed_c.push_back(c);
        observed_v.push_back(val);
    }));
    EXPECT_EQ(observed_r, expected_r);
    EXPECT_EQ(observed_c, expected_c);
    EXPECT_EQ(observed_v, expected_v);
}

INSTANTIATE_TEST_SUITE_P(
    ParserCoordinateMatrix,
    ParserCoordinateMatrixTest,
    ::testing::Values(
        std::make_tuple<std::string, int, int, std::vector<int>, std::vector<int>, std::vector<int> >(
            "1 11 1\n22 2 23\n3 333 456\n444 4 7890\n", // trailing newline
            500,
            500,
            { 1, 22, 3, 444 },
            { 11, 2, 333, 4 },
            { 1, 23, 456, 7890 }
        ),
        std::make_tuple<std::string, int, int, std::vector<int>, std::vector<int>, std::vector<int> >(
            "1 19 10\n29 122 230\n39 33 4560\n494 94 789", // no trailing newline
            500,
            500,
            { 1, 29, 39, 494 },
            { 19, 122, 33, 94 },
            { 10, 230, 4560, 789 }
        ),
        std::make_tuple<std::string, int, int, std::vector<int>, std::vector<int>, std::vector<int> >(
            "191 121 7\n422 102 8\n73 33 9\n484 46 10  ", // trailing blank but no trailing newline
            500,
            500,
            { 191, 422, 73, 484 },
            { 121, 102, 33, 46 },
            { 7, 8, 9, 10 }
        ),
        std::make_tuple<std::string, int, int, std::vector<int>, std::vector<int>, std::vector<int> >(
            "   1 11 12   \n\t22 \t2   345\t\n3\t\t333     67  \n444  4   890\n", // variable numbers of blanks
            500,
            500,
            { 1, 22, 3, 444 },
            { 11, 2, 333, 4 },
            { 12, 345, 67, 890 }
        ),
        std::make_tuple<std::string, int, int, std::vector<int>, std::vector<int>, std::vector<int> >(
            "\n\n19 29 9\n%iamacomment\n28 3 13\n%another comment\n\n65 44 43\n98 7 5\n", // comments, newlines and crap. 
            500,
            500,
            { 19, 28, 65, 98 },
            { 29, 3, 44, 7},
            { 9, 13, 43, 5 }
        ),
        std::make_tuple<std::string, int, int, std::vector<int>, std::vector<int>, std::vector<int> >(
            "19 29 9\n28 3 13\n65 44 43\n98 7 5\n%this is the end", // ends on a comment
            500,
            500,
            { 19, 28, 65, 98 },
            { 29, 3, 44, 7},
            { 9, 13, 43, 5 }
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

TEST(ParserCoordinateMatrix, Errors) {
    test_error("%%MatrixMarket matrix coordinate integer general\n1 1 1\n ", "expected two size fields");
    test_error("%%MatrixMarket matrix coordinate integer general\n1 1 1\n1", "unexpected end of file");
    test_error("%%MatrixMarket matrix coordinate integer general\n1 1 1\n1 ", "unexpected end of file");
    test_error("%%MatrixMarket matrix coordinate integer general\n1 1 1\n1\n", "unexpected newline");
    test_error("%%MatrixMarket matrix coordinate integer general\n1 1 1\n1 \n", "empty index field");
    test_error("%%MatrixMarket matrix coordinate integer general\n1 1 1\n1 1", "unexpected end of file");
    test_error("%%MatrixMarket matrix coordinate integer general\n1 1 1\n1 1 ", "unexpected end of file");
    test_error("%%MatrixMarket matrix coordinate integer general\n1 1 1\n1 1\n", "unexpected newline");
    test_error("%%MatrixMarket matrix coordinate integer general\n1 1 1\n1 1  \n", "empty integer");

    test_error("%%MatrixMarket matrix coordinate integer general\n2 2 1\n1 1 1\n2 2 2", "more lines present");
    test_error("%%MatrixMarket matrix coordinate integer general\n1 1 1\n0 1 1", "row index must be positive");
    test_error("%%MatrixMarket matrix coordinate integer general\n1 1 1\n2 1 1", "row index out of range");
    test_error("%%MatrixMarket matrix coordinate integer general\n1 1 1\n1 0 1", "column index must be positive");
    test_error("%%MatrixMarket matrix coordinate integer general\n1 1 1\n1 2 1", "column index out of range");

    test_error("%%MatrixMarket matrix coordinate integer general\n2 2 2\n1 1 1\n", "fewer lines present");

    {
        std::string input = "%%MatrixMarket matrix coordinate integer general\n1 1 1\n1 1 1";
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

TEST(ParserCoordinateMatrix, Types) {
    { // Checking it works with real.
        std::string input = "%%MatrixMarket matrix coordinate real general\n10 10 3\n1 5 1.2e-4\n5 1 -12.34\n2 2 inf\n";
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
        parser.scan_preamble();

        const auto& deets = parser.get_banner();
        EXPECT_EQ(deets.object, eminem::Object::MATRIX);
        EXPECT_EQ(deets.format, eminem::Format::COORDINATE);
        EXPECT_EQ(deets.field, eminem::Field::REAL);
        EXPECT_EQ(deets.symmetry, eminem::Symmetry::GENERAL);

        EXPECT_EQ(parser.get_nrows(), 10);
        EXPECT_EQ(parser.get_ncols(), 10);
        EXPECT_EQ(parser.get_nlines(), 3);

        std::vector<int> observed_r, observed_c;
        std::vector<double> observed_v;
        EXPECT_TRUE(parser.scan_real([&](size_t r, size_t c, double val) {
            observed_r.push_back(r);
            observed_c.push_back(c);
            observed_v.push_back(val);
        }));

        std::vector<int> expected_r { 1, 5, 2 };
        EXPECT_EQ(observed_r, expected_r);
        std::vector<int> expected_c { 5, 1, 2 };
        EXPECT_EQ(observed_c, expected_c);

        std::vector<double> expected_v { 1.2e-4, -12.34, std::numeric_limits<double>::infinity() };
        ASSERT_EQ(observed_v.size(), expected_v.size());
        for (size_t i = 0; i < observed_v.size(); ++i) {
            EXPECT_DOUBLE_EQ(observed_v[i], expected_v[i]);
        }
    }

    { // Checking it works with complex.
        std::string input = "%%MatrixMarket matrix coordinate complex general\n10 10 3\n1 5 78 1.2e-4\n5 1 12.34 -9.9\n2 2 inf 10\n";
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
        parser.scan_preamble();

        const auto& deets = parser.get_banner();
        EXPECT_EQ(deets.object, eminem::Object::MATRIX);
        EXPECT_EQ(deets.format, eminem::Format::COORDINATE);
        EXPECT_EQ(deets.field, eminem::Field::COMPLEX);
        EXPECT_EQ(deets.symmetry, eminem::Symmetry::GENERAL);

        EXPECT_EQ(parser.get_nrows(), 10);
        EXPECT_EQ(parser.get_ncols(), 10);
        EXPECT_EQ(parser.get_nlines(), 3);

        std::vector<int> observed_r, observed_c;
        std::vector<std::complex<double> > observed_v;
        EXPECT_TRUE(parser.scan_complex([&](size_t r, size_t c, std::complex<double> val) {
            observed_r.push_back(r);
            observed_c.push_back(c);
            observed_v.push_back(val);
        }));

        std::vector<int> expected_r { 1, 5, 2 };
        EXPECT_EQ(observed_r, expected_r);
        std::vector<int> expected_c { 5, 1, 2 };
        EXPECT_EQ(observed_c, expected_c);

        std::vector<std::complex<double> > expected_v { { 78, 1.2e-4 } , { 12.34, -9.9 }, { std::numeric_limits<double>::infinity(), 10 } };
        ASSERT_EQ(observed_v.size(), expected_v.size());
        for (size_t i = 0; i < observed_v.size(); ++i) {
            EXPECT_DOUBLE_EQ(observed_v[i].real(), expected_v[i].real());
            EXPECT_DOUBLE_EQ(observed_v[i].imag(), expected_v[i].imag());
        }
    }
}

TEST(ParserCoordinateMatrix, QuitEarly) {
    std::string input = "%%MatrixMarket matrix coordinate integer general\n10 10 3\n1 2 33\n4 5 666\n7 8 9\n";

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

TEST(ParserCoordinateMatrix, Empty) {
    std::string input = "%%MatrixMarket matrix coordinate integer general\n0 0 0";
    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));

    parser.scan_preamble();

    const auto& deets = parser.get_banner();
    EXPECT_EQ(deets.object, eminem::Object::MATRIX);
    EXPECT_EQ(deets.format, eminem::Format::COORDINATE);
    EXPECT_EQ(deets.field, eminem::Field::INTEGER);
    EXPECT_EQ(deets.symmetry, eminem::Symmetry::GENERAL);

    EXPECT_EQ(parser.get_nrows(), 0);
    EXPECT_EQ(parser.get_ncols(), 0);
    EXPECT_EQ(parser.get_nlines(), 0);

    std::vector<int> observed;
    EXPECT_TRUE(parser.scan_integer([&](size_t, size_t, int val) {
        observed.push_back(val);
    }));
    EXPECT_TRUE(observed.empty());
}
