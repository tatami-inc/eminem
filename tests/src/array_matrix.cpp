#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "byteme/byteme.hpp"
#include "eminem/Parser.hpp"

#include <string>
#include <memory>
#include <cstdint>
#include <limits>

class ParserArrayMatrixTest : public ::testing::TestWithParam<std::tuple<std::string, int, int, std::vector<int> > > {};

TEST_P(ParserArrayMatrixTest, Success) {
    auto param = GetParam();
    const std::string& content = std::get<0>(param);
    int nr = std::get<1>(param);
    int nc = std::get<2>(param);
    const auto& expected_v = std::get<3>(param);

    std::string input = "%%MatrixMarket matrix array integer general\n" + std::to_string(nr) + " " + std::to_string(nc) + "\n" + content;
    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
    parser.scan_preamble();

    const auto& deets = parser.get_banner();
    EXPECT_EQ(deets.object, eminem::Object::MATRIX);
    EXPECT_EQ(deets.format, eminem::Format::ARRAY);
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

    std::vector<int> expected_r, expected_c;
    for (int c = 1; c <= nc; ++c) {
        expected_c.insert(expected_c.end(), nr, c);
        for (int r = 1; r <= nr; ++r) {
            expected_r.push_back(r);
        }
    }
    EXPECT_EQ(observed_r, expected_r);
    EXPECT_EQ(observed_c, expected_c);
    EXPECT_EQ(observed_v, expected_v);
}

INSTANTIATE_TEST_SUITE_P(
    ParserArrayMatrix,
    ParserArrayMatrixTest,
    ::testing::Values(
        std::make_tuple<std::string, int, int, std::vector<int> >(
            "111\n0\n22\n3333\n4\n5\n66\n7\n888\n9\n0\n1\n",
            4,
            3,
            { 111, 0, 22, 3333, 4, 5, 66, 7, 888, 9, 0, 1 }
        ),
        std::make_tuple<std::string, int, int, std::vector<int> >(
            "1\n19\n10\n29\n122\n230\n39\n33\n4560\n494\n94\n789", // no trailing newline
            3,
            4,
            { 1, 19, 10, 29, 122, 230, 39, 33, 4560, 494, 94, 789 }
        ),
        std::make_tuple<std::string, int, int, std::vector<int> >(
            "191\n121\n7\n422\n102\n8  ", // trailing blank but no trailing newline
            2,
            3,
            { 191, 121, 7, 422, 102, 8 }
        ),
        std::make_tuple<std::string, int, int, std::vector<int> >(
            "   1 \n11\n 12   \n\t22\n \t2  \n 345\t\n3\t\t\n333  \n   67  \n444 \n 4\n  890\n", // variable numbers of blanks
            4,
            3,
            { 1, 11, 12, 22, 2, 345, 3, 333, 67, 444, 4, 890 }
        ),
        std::make_tuple<std::string, int, int, std::vector<int> >(
            "\n\n19\n29\n9\n%iamacomment\n28\n3\n13\n%another comment\n\n65\n44\n43\n98\n7\n5\n\n%morecommentary\n\n981\n12\n77\n", // comments, newlines and crap. 
            5,
            3,
            { 19, 29, 9, 28, 3, 13, 65, 44, 43, 98, 7, 5, 981, 12, 77 }
        ),
        std::make_tuple<std::string, int, int, std::vector<int> >(
            "19\n29\n9\n28\n3\n13\n65\n44\n43\n%this is the end", // ends on a comment
            3,
            3,
            { 19, 29, 9, 28, 3, 13, 65, 44, 43 }
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

TEST(ParserArrayMatrix, Errors) {
    test_error("%%MatrixMarket matrix array integer general\n1 1\n ", "expected at least one field");
    test_error("%%MatrixMarket matrix array integer general\n1 1\n1\n2", "more lines present");
    test_error("%%MatrixMarket matrix array integer general\n2 2\n1\n", "fewer lines present");
}

TEST(ParserArrayMatrix, Types) {
    { // Checking it works with real.
        std::string input = "%%MatrixMarket matrix array real general\n2 3\n1\n1.2e-4\n5.1\n-12.34\n2.2\ninf\n";
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
        parser.scan_preamble();

        const auto& deets = parser.get_banner();
        EXPECT_EQ(deets.object, eminem::Object::MATRIX);
        EXPECT_EQ(deets.format, eminem::Format::ARRAY);
        EXPECT_EQ(deets.field, eminem::Field::REAL);
        EXPECT_EQ(deets.symmetry, eminem::Symmetry::GENERAL);

        EXPECT_EQ(parser.get_nrows(), 2);
        EXPECT_EQ(parser.get_ncols(), 3);
        EXPECT_EQ(parser.get_nlines(), 6);

        std::vector<int> observed_r, observed_c;
        std::vector<double> observed_v;
        EXPECT_TRUE(parser.scan_real([&](size_t r, size_t c, double val) {
            observed_r.push_back(r);
            observed_c.push_back(c);
            observed_v.push_back(val);
        }));

        std::vector<int> expected_r { 1, 2, 1, 2, 1, 2 };
        EXPECT_EQ(observed_r, expected_r);
        std::vector<int> expected_c { 1, 1, 2, 2, 3, 3 };
        EXPECT_EQ(observed_c, expected_c);

        std::vector<double> expected_v { 1, 1.2e-4, 5.1, -12.34, 2.2, std::numeric_limits<double>::infinity() };
        ASSERT_EQ(observed_v.size(), expected_v.size());
        for (size_t i = 0; i < observed_v.size(); ++i) {
            EXPECT_DOUBLE_EQ(observed_v[i], expected_v[i]);
        }
    }

    { // Checking it works with complex.
        std::string input = "%%MatrixMarket matrix array complex general\n3 2\n1 5\n78 1.2e-4\n5 1\n12.34 -9.9\n2 2\ninf 10\n";
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
        parser.scan_preamble();

        const auto& deets = parser.get_banner();
        EXPECT_EQ(deets.object, eminem::Object::MATRIX);
        EXPECT_EQ(deets.format, eminem::Format::ARRAY);
        EXPECT_EQ(deets.field, eminem::Field::COMPLEX);
        EXPECT_EQ(deets.symmetry, eminem::Symmetry::GENERAL);

        EXPECT_EQ(parser.get_nrows(), 3);
        EXPECT_EQ(parser.get_ncols(), 2);
        EXPECT_EQ(parser.get_nlines(), 6);

        std::vector<int> observed_r, observed_c;
        std::vector<std::complex<double> > observed_v;
        EXPECT_TRUE(parser.scan_complex([&](size_t r, size_t c, std::complex<double> val) {
            observed_r.push_back(r);
            observed_c.push_back(c);
            observed_v.push_back(val);
        }));

        std::vector<int> expected_r { 1, 2, 3, 1, 2, 3 };
        EXPECT_EQ(observed_r, expected_r);
        std::vector<int> expected_c { 1, 1, 1, 2, 2, 2 };
        EXPECT_EQ(observed_c, expected_c);

        std::vector<std::complex<double> > expected_v { { 1, 5 }, { 78, 1.2e-4 }, { 5, 1 }, { 12.34, -9.9 }, { 2, 2 }, { std::numeric_limits<double>::infinity(), 10 } };
        ASSERT_EQ(observed_v.size(), expected_v.size());
        for (size_t i = 0; i < observed_v.size(); ++i) {
            EXPECT_DOUBLE_EQ(observed_v[i].real(), expected_v[i].real());
            EXPECT_DOUBLE_EQ(observed_v[i].imag(), expected_v[i].imag());
        }
    }
}

TEST(ParserArrayMatrix, QuitEarly) {
    std::string input = "%%MatrixMarket matrix array integer general\n3 1\n1233\n45666\n7890\n";

    { // quits immediately.
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
        parser.scan_preamble();

        std::vector<int> observed;
        EXPECT_FALSE(parser.scan_integer([&](size_t, size_t, int val) -> bool {
            observed.push_back(val);
            return false;
        }));
        std::vector<int> expected { 1233 };
        EXPECT_EQ(observed, expected);
    }

    { // never quits but the function still returns a value.
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
        parser.scan_preamble();

        std::vector<int> observed;
        EXPECT_TRUE(parser.scan_integer([&](size_t, size_t, int val) -> bool {
            observed.push_back(val);
            return true;
        }));
        std::vector<int> expected { 1233, 45666, 7890 };
        EXPECT_EQ(observed, expected);
    }
}

TEST(ParserArrayMatrix, Empty) {
    std::string input = "%%MatrixMarket matrix array integer general\n0 0";
    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));

    parser.scan_preamble();

    const auto& deets = parser.get_banner();
    EXPECT_EQ(deets.object, eminem::Object::MATRIX);
    EXPECT_EQ(deets.format, eminem::Format::ARRAY);
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
