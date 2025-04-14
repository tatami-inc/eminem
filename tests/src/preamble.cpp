#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "byteme/byteme.hpp"
#include "eminem/Parser.hpp"

#include <string>
#include <memory>

/**********************************************
 **********************************************/

class ParserPreambleCoordinateMatrixTest : public ::testing::TestWithParam<std::tuple<std::string, int, int, int> > {};

TEST_P(ParserPreambleCoordinateMatrixTest, Success) {
    auto param = GetParam();
    const std::string& input = std::get<0>(param);
    int nr = std::get<1>(param);
    int nc = std::get<2>(param);
    int nl = std::get<3>(param);

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
    EXPECT_EQ(parser.get_nlines(), nl);
}

INSTANTIATE_TEST_SUITE_P(
    ParserPreamble,
    ParserPreambleCoordinateMatrixTest,
    ::testing::Values(
        std::tuple<std::string, int, int, int>("%%MatrixMarket matrix coordinate integer general\n5 2 1", 5, 2, 1), // no trailing newline
        std::tuple<std::string, int, int, int>("%%MatrixMarket matrix coordinate integer general\n5 2 1   ", 5, 2, 1), // trailing blank but no trailing newline
        std::tuple<std::string, int, int, int>("%%MatrixMarket matrix coordinate integer general\n69 2 43\n", 69, 2, 43), // trailing newline
        std::tuple<std::string, int, int, int>("%%MatrixMarket matrix coordinate integer general\n%other comment\n%morestuff\n\n5 124 27\n", 5, 124, 27), // comments and empty lines
        std::tuple<std::string, int, int, int>("%%MatrixMarket  matrix\tcoordinate    integer  general    \n\t19\t 110 \t88\t\n", 19, 110, 88), // extra blanks
        std::tuple<std::string, int, int, int>("%%MatrixMarket matrix coordinate integer general foobar\n123 45 67890\n", 123, 45, 67890) // extra fields
    )
);

/**********************************************
 **********************************************/

class ParserPreambleCoordinateVectorTest : public ::testing::TestWithParam<std::tuple<std::string, int, int> > {};

TEST_P(ParserPreambleCoordinateVectorTest, Success) {
    auto param = GetParam();
    const std::string& input = std::get<0>(param);
    int nr = std::get<1>(param);
    int nl = std::get<2>(param);

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
    EXPECT_EQ(parser.get_nlines(), nl);
}

INSTANTIATE_TEST_SUITE_P(
    ParserPreamble,
    ParserPreambleCoordinateVectorTest,
    ::testing::Values(
        std::tuple<std::string, int, int>("%%MatrixMarket vector coordinate integer general\n50 21", 50, 21), // no trailing newline
        std::tuple<std::string, int, int>("%%MatrixMarket vector coordinate integer general\n50 21 \t ", 50, 21), // trailing blank but no trailing newline
        std::tuple<std::string, int, int>("%%MatrixMarket vector coordinate integer general\n69 23\n", 69, 23), // trailing newline
        std::tuple<std::string, int, int>("%%MatrixMarket vector coordinate integer general\n\n%other comment\n% FOO BAR\n5124 27\n", 5124, 27), // comments and empty lines
        std::tuple<std::string, int, int>("%%MatrixMarket   vector \tcoordinate    integer  general    \n  1910 \t88\t\n", 1910, 88), // extra blanks
        std::tuple<std::string, int, int>("%%MatrixMarket vector coordinate integer symmetric foobar\n123456 7890\n", 123456, 7890) // extra fields (symmetry is ignored)
    )
);

/**********************************************
 **********************************************/

class ParserPreambleArrayMatrixTest : public ::testing::TestWithParam<std::tuple<std::string, int, int> > {};

TEST_P(ParserPreambleArrayMatrixTest, Success) {
    auto param = GetParam();
    const std::string& input = std::get<0>(param);
    int nr = std::get<1>(param);
    int nc = std::get<2>(param);

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
    EXPECT_EQ(parser.get_nlines(), static_cast<size_t>(nr) * static_cast<size_t>(nc));
}

INSTANTIATE_TEST_SUITE_P(
    ParserPreamble,
    ParserPreambleArrayMatrixTest,
    ::testing::Values(
        std::tuple<std::string, int, int>("%%MatrixMarket matrix array integer general\n5 2", 5, 2), // no trailing newline
        std::tuple<std::string, int, int>("%%MatrixMarket matrix array integer general\n5 2\t", 5, 2), // trailing blank but no trailing newline
        std::tuple<std::string, int, int>("%%MatrixMarket matrix array integer general\n69 243\n", 69, 243), // trailing newline
        std::tuple<std::string, int, int>("%%MatrixMarket matrix array integer general\n%other comment\n%morestuff\n\n512 427\n", 512, 427), // comments and empty lines
        std::tuple<std::string, int, int>("%%MatrixMarket\t\tmatrix\tarray    integer  general    \n\t\t 110 \t88\t\n", 110, 88), // extra blanks
        std::tuple<std::string, int, int>("%%MatrixMarket matrix array integer general foobar\n12345 67890\n", 12345, 67890) // extra fields
    )
);

/**********************************************
 **********************************************/

class ParserPreambleArrayVectorTest : public ::testing::TestWithParam<std::tuple<std::string, int> > {};

TEST_P(ParserPreambleArrayVectorTest, Success) {
    auto param = GetParam();
    const std::string& input = std::get<0>(param);
    int nr = std::get<1>(param);

    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
    parser.scan_preamble();

    const auto& deets = parser.get_banner();
    EXPECT_EQ(deets.object, eminem::Object::VECTOR);
    EXPECT_EQ(deets.format, eminem::Format::ARRAY);
    EXPECT_EQ(deets.field, eminem::Field::INTEGER);
    EXPECT_EQ(deets.symmetry, eminem::Symmetry::GENERAL);

    EXPECT_EQ(parser.get_nrows(), nr);
    EXPECT_EQ(parser.get_ncols(), 1);
    EXPECT_EQ(parser.get_nlines(), nr);
}

INSTANTIATE_TEST_SUITE_P(
    ParserPreamble,
    ParserPreambleArrayVectorTest,
    ::testing::Values(
        std::tuple<std::string, int>("%%MatrixMarket vector array integer general\n5", 5), // no trailing newline
        std::tuple<std::string, int>("%%MatrixMarket vector array integer general\n5  ", 5), // trailing blanks but no trailing newline
        std::tuple<std::string, int>("%%MatrixMarket vector array integer general\n69243\n", 69243), // trailing newline
        std::tuple<std::string, int>("%%MatrixMarket vector array integer general\n%other comment\n%morestuff\n\n512427\n", 512427), // comments and empty lines
        std::tuple<std::string, int>("%%MatrixMarket  vector    array    integer  general    \n\t\t 11088\t\n", 11088), // extra blanks
        std::tuple<std::string, int>("%%MatrixMarket vector array integer general foobar\n1234567890\n", 1234567890) // extra fields
    )
);

/**********************************************
 **********************************************/

static void test_error(const std::string& input, std::string msg) {
    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
    EXPECT_ANY_THROW({
        try {
            parser.scan_preamble();
        } catch (std::exception& e) {
            EXPECT_THAT(e.what(), ::testing::HasSubstr(msg));
            throw;
        }
    });
}

TEST(ParserPreamble, GeneralErrors) {
    test_error("", "failed to find banner");
    test_error("MatrixMarket", "first line of the file");
    test_error("%%Matrix", "end of file reached before matching the banner");
    test_error("%%MatrixMarketey", "first line of the file");
    test_error("%%MatrixMarket\n", "end of file reached before matching the banner");
    test_error("%%MatrixMarket\nwhee", "end of line reached before matching the banner");
    test_error("% %MatrixMarket matrix coordinate real symmetric\n5 2 1", "first line");
}

TEST(ParserPreamble, ObjectErrors) {
    test_error("%%MatrixMarket foobar array integer general\n100 200\n", "first banner field should be one of");
    test_error("%%MatrixMarket matrix", "end of file reached after the first banner field");
    test_error("%%MatrixMarket matrix  \t ", "end of file reached after the first banner field");
    test_error("%%MatrixMarket matrix\n", "end of file reached after the first banner field");
    test_error("%%MatrixMarket matrix\nwhee", "end of line reached after the first banner field");
}

TEST(ParserPreamble, FormatErrors) {
    test_error("%%MatrixMarket matrix foobar integer general\n100 200\n", "second banner field should be one of");
    test_error("%%MatrixMarket matrix array", "end of file reached after the second banner field");
    test_error("%%MatrixMarket matrix array    ", "end of file reached after the second banner field");
    test_error("%%MatrixMarket matrix array\n", "end of file reached after the second banner field");
    test_error("%%MatrixMarket matrix array\nwhee", "end of line reached after the second banner field");
}

TEST(ParserPreamble, SizeErrors) {
    test_error("%%MatrixMarket matrix coordinate integer general\n5 2 ", "unexpected end of file");
    test_error("%%MatrixMarket matrix coordinate integer general\n5", "unexpected end of file");
    test_error("%%MatrixMarket matrix coordinate integer general\n5 2\n", "unexpected newline");
    test_error("%%MatrixMarket matrix coordinate integer general\n5 2 \n", "empty size field");
    test_error("%%MatrixMarket matrix coordinate integer general\n5 2 1 3\n", "expected newline after");
    test_error("%%MatrixMarket matrix coordinate integer general\n\n", "failed to find size line");
    test_error("%%MatrixMarket matrix coordinate integer general\n \n", "empty size field");

    test_error("%%MatrixMarket vector coordinate integer general\n5 1 3\n", "expected newline after");
    test_error("%%MatrixMarket vector coordinate integer general\n5\n", "unexpected newline");
    test_error("%%MatrixMarket vector coordinate integer general\n5 \n", "empty size field");
    test_error("%%MatrixMarket vector coordinate integer general\n5", "unexpected end of file");
    test_error("%%MatrixMarket vector coordinate integer general\n\n", "failed to find size line");
    test_error("%%MatrixMarket vector coordinate integer general\n   \n", "empty size field");

    test_error("%%MatrixMarket matrix array integer general\n5", "unexpected end of file");
    test_error("%%MatrixMarket matrix array integer general\n5\n", "unexpected newline");
    test_error("%%MatrixMarket matrix array integer general\n5 \n", "empty size field");
    test_error("%%MatrixMarket matrix array integer general\n5 2 1\n", "expected newline after");
    test_error("%%MatrixMarket matrix array integer general\n\n", "failed to find size line");
    test_error("%%MatrixMarket matrix array integer general\n \n", "empty size field");

    test_error("%%MatrixMarket vector array integer general\n\n", "failed to find size line");
    test_error("%%MatrixMarket vector array integer general\n\t\t\n", "empty size field");

    test_error("%%MatrixMarket matrix coordinate integer general\na 2 1\n", "unexpected character");
    test_error("%%MatrixMarket matrix coordinate integer general\n2 -5 1\n", "unexpected character");
}

/**********************************************
 **********************************************/

class ParserPreambleFieldTest : public ::testing::TestWithParam<std::tuple<std::string, eminem::Field> > {};

TEST_P(ParserPreambleFieldTest, Success) {
    auto param = GetParam();
    std::string input = std::get<0>(param);
    auto field = std::get<1>(param);

    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
    parser.scan_preamble();

    const auto& deets = parser.get_banner();
    EXPECT_EQ(deets.object, eminem::Object::MATRIX);
    EXPECT_EQ(deets.format, eminem::Format::COORDINATE);
    EXPECT_EQ(deets.field, field);
    EXPECT_EQ(deets.symmetry, eminem::Symmetry::GENERAL);

    EXPECT_EQ(parser.get_nrows(), 34);
    EXPECT_EQ(parser.get_ncols(), 67);
    EXPECT_EQ(parser.get_nlines(), 19);
}

INSTANTIATE_TEST_SUITE_P(
    ParserPreamble,
    ParserPreambleFieldTest,
    ::testing::Values(
        std::tuple<std::string, eminem::Field>("%%MatrixMarket matrix coordinate integer general\n34 67 19\n", eminem::Field::INTEGER),
        std::tuple<std::string, eminem::Field>("%%MatrixMarket matrix coordinate real general\n34 67 19\n", eminem::Field::REAL),
        std::tuple<std::string, eminem::Field>("%%MatrixMarket matrix coordinate double general\n34 67 19\n", eminem::Field::DOUBLE),
        std::tuple<std::string, eminem::Field>("%%MatrixMarket matrix coordinate complex general\n34 67 19\n", eminem::Field::COMPLEX),
        std::tuple<std::string, eminem::Field>("%%MatrixMarket matrix coordinate pattern general\n34 67 19\n", eminem::Field::PATTERN)
    )
);

TEST(ParserPreamble, FieldErrors) {
    test_error("%%MatrixMarket matrix array foobar general\n100 200\n", "third banner field should be one of");
    test_error("%%MatrixMarket matrix array integer", "end of file reached after the third banner field");
    test_error("%%MatrixMarket matrix array integer   ", "end of file reached after the third banner field");
    test_error("%%MatrixMarket matrix array integer\n", "end of file reached after the third banner field");
    test_error("%%MatrixMarket matrix array integer\n\n", "end of line reached after the third banner field");
}

/**********************************************
 **********************************************/

class ParserPreambleSymmetryTest : public ::testing::TestWithParam<std::tuple<std::string, eminem::Symmetry> > {};

TEST_P(ParserPreambleSymmetryTest, Success) {
    auto param = GetParam();
    std::string input = std::get<0>(param);
    auto symmetry = std::get<1>(param);

    auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
    eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
    parser.scan_preamble();

    const auto& deets = parser.get_banner();
    EXPECT_EQ(deets.object, eminem::Object::MATRIX);
    EXPECT_EQ(deets.format, eminem::Format::ARRAY);
    EXPECT_EQ(deets.symmetry, symmetry);

    EXPECT_EQ(parser.get_nrows(), 200);
    EXPECT_EQ(parser.get_ncols(), 100);
    EXPECT_EQ(parser.get_nlines(), 20000);
}

INSTANTIATE_TEST_SUITE_P(
    ParserPreamble,
    ParserPreambleSymmetryTest,
    ::testing::Values(
        std::tuple<std::string, eminem::Symmetry>("%%MatrixMarket matrix array integer general\n200 100\n", eminem::Symmetry::GENERAL),
        std::tuple<std::string, eminem::Symmetry>("%%MatrixMarket matrix array real skew-symmetric\n200 100\n", eminem::Symmetry::SKEW_SYMMETRIC),
        std::tuple<std::string, eminem::Symmetry>("%%MatrixMarket matrix array double symmetric\n200 100\n", eminem::Symmetry::SYMMETRIC),
        std::tuple<std::string, eminem::Symmetry>("%%MatrixMarket matrix array complex hermitian\n200 100\n", eminem::Symmetry::HERMITIAN)
    )
);

TEST(ParserPreamble, SymmetryErrors) {
    test_error("%%MatrixMarket matrix array integer foobar\n100 200\n", "fourth banner field should be one of");
    test_error("%%MatrixMarket matrix array integer general", "end of file reached after the fourth banner field");
    test_error("%%MatrixMarket matrix array integer general  ", "end of file reached after the fourth banner field");
}

/**********************************************
 **********************************************/

TEST(ParserPreamble, ScanErrors) {
    std::string input = "%%MatrixMarket matrix coordinate integer general\n5 2 5\n";
    {
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
        EXPECT_ANY_THROW({
            try {
                parser.get_banner();
            } catch (std::exception& e) {
                EXPECT_THAT(e.what(), ::testing::HasSubstr("banner has not yet been scanned"));
                throw;
            }
        });
    }

    {
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
        EXPECT_ANY_THROW({
            try {
                parser.scan_preamble();
                parser.scan_preamble();
            } catch (std::exception& e) {
                EXPECT_THAT(e.what(), ::testing::HasSubstr("banner has already been scanned"));
                throw;
            }
        });
    }

    {
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
        EXPECT_ANY_THROW({
            try {
                parser.get_nrows();
            } catch (std::exception& e) {
                EXPECT_THAT(e.what(), ::testing::HasSubstr("size line has not yet been scanned"));
                throw;
            }
        });
    }

    {
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
        EXPECT_ANY_THROW({
            try {
                parser.get_ncols();
            } catch (std::exception& e) {
                EXPECT_THAT(e.what(), ::testing::HasSubstr("size line has not yet been scanned"));
                throw;
            }
        });
    }

    {
        auto reader = std::make_unique<byteme::RawBufferReader>(reinterpret_cast<const unsigned char*>(input.data()), input.size()); 
        eminem::Parser parser(std::make_unique<byteme::PerByteSerial<char> >(std::move(reader)));
        EXPECT_ANY_THROW({
            try {
                parser.get_nlines();
            } catch (std::exception& e) {
                EXPECT_THAT(e.what(), ::testing::HasSubstr("size line has not yet been scanned"));
                throw;
            }
        });
    }
}
