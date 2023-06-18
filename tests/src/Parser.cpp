#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "byteme/Reader.hpp"
#include "eminem/Parser.hpp"

#include "simulate.h"

#include <limits>
#include <string>
#include <vector>

struct ChunkedBufferReader : public byteme::Reader {
    ChunkedBufferReader(const char* p, size_t n, size_t c) : ChunkedBufferReader(reinterpret_cast<const unsigned char*>(p), n, c) {}

    ChunkedBufferReader(const unsigned char* p, size_t n, size_t c) : source(p), len(n), chunksize(c) {
        position = -chunksize;        
    }

public:
    bool operator()() {
        position += chunksize; 
        return (position + chunksize < len); // i.e., next call will be invalid.
    }

    const unsigned char * buffer () const {
        return source + position;
    }

    size_t available() const {
        return std::min(chunksize, len - position);
    }

private:
    const unsigned char* source;
    size_t len;
    size_t position;
    size_t chunksize;
};

/*****************************************
 *****************************************/

class ParserPreambleTest : public ::testing::TestWithParam<int> {
protected:
    void test_error(const std::string& input, std::string msg, int chunksize) {
        EXPECT_ANY_THROW({
            try {
                ChunkedBufferReader reader(input.data(), input.size(), chunksize);
                eminem::Parser parser(reader);
                parser.scan_preamble();
            } catch (std::exception& e) {
                EXPECT_THAT(e.what(), ::testing::HasSubstr(msg));
                throw;
            }
        });
    }
};

TEST_P(ParserPreambleTest, Success) {
    auto chunksize = GetParam();

    {
        std::string input = "%MatrixMarket matrix coordinate real symmetric\n5 2 1";
        ChunkedBufferReader reader(input.data(), input.size(), chunksize);
        eminem::Parser parser(reader);
        parser.scan_preamble();

        const auto& deets = parser.get_banner();
        EXPECT_EQ(deets.object, eminem::Object::MATRIX);
        EXPECT_EQ(deets.format, eminem::Format::COORDINATE);
        EXPECT_EQ(deets.field, eminem::Field::REAL);
        EXPECT_EQ(deets.symmetry, eminem::Symmetry::SYMMETRIC);

        EXPECT_EQ(parser.get_nrows(), 5);
        EXPECT_EQ(parser.get_ncols(), 2);
        EXPECT_EQ(parser.get_nlines(), 1);
    }

    {
        std::string input = "%MatrixMarket matrix array integer general\n235 122\n";
        ChunkedBufferReader reader(input.data(), input.size(), chunksize);
        eminem::Parser parser(reader);
        parser.scan_preamble();

        const auto& deets = parser.get_banner();
        EXPECT_EQ(deets.object, eminem::Object::MATRIX);
        EXPECT_EQ(deets.format, eminem::Format::ARRAY);
        EXPECT_EQ(deets.field, eminem::Field::INTEGER);
        EXPECT_EQ(deets.symmetry, eminem::Symmetry::GENERAL);

        EXPECT_EQ(parser.get_nrows(), 235);
        EXPECT_EQ(parser.get_ncols(), 122);
        EXPECT_EQ(parser.get_nlines(), 235 * 122);
    }

    {
        std::string input = "%%MatrixMarket vector array double skew-symmetric\n52";
        ChunkedBufferReader reader(input.data(), input.size(), chunksize);
        eminem::Parser parser(reader);
        parser.scan_preamble();

        const auto& deets = parser.get_banner();
        EXPECT_EQ(deets.object, eminem::Object::VECTOR);
        EXPECT_EQ(deets.format, eminem::Format::ARRAY);
        EXPECT_EQ(deets.field, eminem::Field::DOUBLE);
        EXPECT_EQ(deets.symmetry, eminem::Symmetry::SKEW_SYMMETRIC);

        EXPECT_EQ(parser.get_nrows(), 52);
        EXPECT_EQ(parser.get_ncols(), 0);
        EXPECT_EQ(parser.get_nlines(), 52);
    }

    {
        std::string input = "%%MatrixMarket matrix coordinate complex hermitian\n99 100 352\n";
        ChunkedBufferReader reader(input.data(), input.size(), chunksize);
        eminem::Parser parser(reader);
        parser.scan_preamble();

        const auto& deets = parser.get_banner();
        EXPECT_EQ(deets.object, eminem::Object::MATRIX);
        EXPECT_EQ(deets.format, eminem::Format::COORDINATE);
        EXPECT_EQ(deets.field, eminem::Field::COMPLEX);
        EXPECT_EQ(deets.symmetry, eminem::Symmetry::HERMITIAN);

        EXPECT_EQ(parser.get_nrows(), 99);
        EXPECT_EQ(parser.get_ncols(), 100);
        EXPECT_EQ(parser.get_nlines(), 352);
    }

    {
        std::string input = "%%MatrixMarket vector coordinate pattern general\n% FOOBAR% WHEE\n100 5\n";
        ChunkedBufferReader reader(input.data(), input.size(), chunksize);
        eminem::Parser parser(reader);
        parser.scan_preamble();

        const auto& deets = parser.get_banner();
        EXPECT_EQ(deets.object, eminem::Object::VECTOR);
        EXPECT_EQ(deets.format, eminem::Format::COORDINATE);
        EXPECT_EQ(deets.field, eminem::Field::PATTERN);
        EXPECT_EQ(deets.symmetry, eminem::Symmetry::GENERAL);

        EXPECT_EQ(parser.get_nrows(), 100);
        EXPECT_EQ(parser.get_ncols(), 0);
        EXPECT_EQ(parser.get_nlines(), 5);
    }
}

TEST_P(ParserPreambleTest, Errors) {
    auto chunksize = GetParam();

    test_error("% %MatrixMarket matrix coordinate real symmetric\n5 2 1", "failed to find banner", chunksize);
    test_error("%matrix coordinate real symmetric\n5 2 1", "failed to find banner", chunksize);
    test_error("%other stuff\n5 2 1\n", "failed to find banner", chunksize);

    test_error("%MatrixMarket foo\n5 2 1\n", "'matrix' or 'vector'", chunksize);
    test_error("%MatrixMarket matrix foo\n5 2 1\n", "'coordinate' or 'array'", chunksize);
    test_error("%MatrixMarket matrix coordinate foo\n5 2 1\n", "third banner field", chunksize);
    test_error("%MatrixMarket matrix coordinate integer foo\n5 2 1\n", "fourth banner field", chunksize);

    test_error("%MatrixMarket matrix coordinate integer general\n5 2\n", "three size fields", chunksize);
    test_error("%MatrixMarket matrix array integer general\n5 2 1\n", "two size fields", chunksize);
    test_error("%MatrixMarket vector array integer general\n5 1\n", "one size field", chunksize);
    test_error("%MatrixMarket vector coordinate integer general\n4\n", "two size fields", chunksize);

    std::string input = "%MatrixMarket matrix coordinate integer general\n5 2 5\n";
    EXPECT_ANY_THROW({
        try {
            ChunkedBufferReader reader(input.data(), input.size(), chunksize);
            eminem::Parser parser(reader);
            parser.get_banner();
        } catch (std::exception& e) {
            EXPECT_THAT(e.what(), ::testing::HasSubstr("banner has not yet been scanned"));
            throw;
        }
    });

    EXPECT_ANY_THROW({
        try {
            ChunkedBufferReader reader(input.data(), input.size(), chunksize);
            eminem::Parser parser(reader);
            parser.get_nlines();
        } catch (std::exception& e) {
            EXPECT_THAT(e.what(), ::testing::HasSubstr("size line has not yet been scanned"));
            throw;
        }
    });
}

INSTANTIATE_TEST_CASE_P(
    Parser,
    ParserPreambleTest,
    ::testing::Values(5, 10, 20, 100)
);

/*****************************************
 *****************************************/

class ParserBodyTest : public ::testing::TestWithParam<int> {};

TEST_P(ParserBodyTest, Integer) {
    auto chunksize = GetParam();

    // Checking coordinates.
    {
        size_t NR = 82, NC = 32;
        auto coords = simulate_coordinates(NR, NC, 0.1);
        auto values = simulate_integers(coords.first.size(), -999, 999);

        std::stringstream stored;
        format_coordinates(stored, NR, NC, coords.first, coords.second, values);
        stored << "\n"; // add an extra newline.
        std::string input = stored.str();

        ChunkedBufferReader reader(input.data(), input.size(), chunksize);
        eminem::Parser parser(reader);
        parser.scan_preamble();

        EXPECT_EQ(parser.get_nrows(), NR);
        EXPECT_EQ(parser.get_ncols(), NC);
        EXPECT_EQ(parser.get_nlines(), values.size());

        std::vector<int> out_rows, out_cols, out_vals;
        parser.scan_integer([&](size_t r, size_t c, int v) -> void {
            out_rows.push_back(r - 1);
            out_cols.push_back(c - 1);
            out_vals.push_back(v);
        });

        EXPECT_EQ(out_rows, coords.first);
        EXPECT_EQ(out_cols, coords.second);
        EXPECT_EQ(out_vals, values);
    }
}

TEST_P(ParserBodyTest, Real) {
    auto chunksize = GetParam();

    // Checking coordinates.
    {
        size_t NR = 82, NC = 32;
        auto coords = simulate_coordinates(NR, NC, 0.1);
        auto values = simulate_real(coords.first.size());

        std::stringstream stored;
        format_coordinates(stored, NR, NC, coords.first, coords.second, values);
        std::string input = stored.str();

        ChunkedBufferReader reader(input.data(), input.size(), chunksize);
        eminem::Parser parser(reader);
        parser.scan_preamble();

        EXPECT_EQ(parser.get_nrows(), NR);
        EXPECT_EQ(parser.get_ncols(), NC);
        EXPECT_EQ(parser.get_nlines(), values.size());

        std::vector<int> out_rows, out_cols;
        std::vector<double> out_vals;
        parser.scan_real([&](size_t r, size_t c, double v) -> void {
            out_rows.push_back(r - 1);
            out_cols.push_back(c - 1);
            out_vals.push_back(v);
        });

        EXPECT_EQ(out_rows, coords.first);
        EXPECT_EQ(out_cols, coords.second);

        EXPECT_EQ(out_vals.size(), values.size());
        for (size_t i = 0; i < out_vals.size(); ++i) {
            EXPECT_TRUE(std::abs(out_vals[i] - values[i]) <= 0.00000001);
        }
    }
}

INSTANTIATE_TEST_CASE_P(
    Parser,
    ParserBodyTest,
    ::testing::Values(5, 20, 100, 1000)
);

