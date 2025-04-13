#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "byteme/byteme.hpp"
#include "eminem/Parser.hpp"

#include <string>
#include <memory>
#include <cstdint>

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
    EXPECT_TRUE(parser.scan_integer([&](size_t, size_t, int val) {
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
            parser.scan_integer([&](size_t, size_t, int){});
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
}

TEST(ParserInteger, OtherType) {
    std::string input = "%%MatrixMarket matrix coordinate integer general\n10 10 3\n1 2 33\n4 5 666\n7 8 9\n";
    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
    parser.scan_preamble();

    std::vector<uint16_t> observed;
    EXPECT_TRUE(parser.template scan_integer<uint16_t>([&](size_t, size_t, uint16_t val) {
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
    EXPECT_FALSE(parser.scan_integer([&](size_t, size_t, int val) -> bool {
        observed.push_back(val);
        return val < 100;
    }));
    std::vector<int> expected { 33, 666 };
    EXPECT_EQ(observed, expected);
}
