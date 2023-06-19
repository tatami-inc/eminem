#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "byteme/Reader.hpp"
#include "eminem/Parser.hpp"

#include "simulate.h"
#include "format.h"

#include <limits>
#include <string>
#include <vector>
#include <complex>

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
        EXPECT_EQ(parser.get_ncols(), 1);
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
        EXPECT_EQ(parser.get_ncols(), 1);
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
    test_error("%MatrixMarket vector coordinate integer general\na 2 1\n", "only non-negative integers", chunksize);
    test_error("%MatrixMarket vector coordinate integer general\n2 -5 1\n", "only non-negative integers", chunksize);

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

class ParserIntegerBodyTest : public ::testing::TestWithParam<int> {};

TEST_P(ParserIntegerBodyTest, CoordinateMatrix) {
    auto chunksize = GetParam();

    size_t NR = 82, NC = 32;
    auto coords = simulate_coordinate(NR, NC, 0.1);
    auto values = simulate_integer(coords.first.size(), -999, 999);

    std::stringstream stored;
    format_coordinate(stored, NR, NC, coords.first, coords.second, values);
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

TEST_P(ParserIntegerBodyTest, CoordinateVector) {
    auto chunksize = GetParam();

    size_t N = 1392;
    auto coords = simulate_coordinate(N, 0.2);
    auto values = simulate_integer(coords.size(), -999, 999);

    std::stringstream stored;
    format_coordinate(stored, N, coords, values); // Don't add the extra newline!
    std::string input = stored.str();

    ChunkedBufferReader reader(input.data(), input.size(), chunksize);
    eminem::Parser parser(reader);
    parser.scan_preamble();

    EXPECT_EQ(parser.get_nrows(), N);
    EXPECT_EQ(parser.get_nlines(), values.size());

    std::vector<int> out_rows, out_cols, out_vals;
    parser.scan_integer([&](size_t r, size_t c, int v) -> void {
        out_rows.push_back(r - 1);
        out_cols.push_back(c - 1);
        out_vals.push_back(v);
    });

    EXPECT_EQ(out_rows, coords);
    EXPECT_EQ(out_cols, std::vector<int>(coords.size()));
    EXPECT_EQ(out_vals, values);
}

TEST_P(ParserIntegerBodyTest, ArrayMatrix) {
    auto chunksize = GetParam();

    size_t NR = 53, NC = 42;
    auto values = simulate_integer(NR * NC, -999, 999);

    std::stringstream stored;
    format_array(stored, NR, NC, values);
    stored << "\n"; // add an extra newline.
    std::string input = stored.str();

    ChunkedBufferReader reader(input.data(), input.size(), chunksize);
    eminem::Parser parser(reader);
    parser.scan_preamble();

    EXPECT_EQ(parser.get_nrows(), NR);
    EXPECT_EQ(parser.get_ncols(), NC);
    EXPECT_EQ(parser.get_nlines(), NR * NC);

    std::vector<int> out_rows, out_cols, out_vals;
    parser.scan_integer([&](size_t r, size_t c, int v) -> void {
        out_rows.push_back(r - 1);
        out_cols.push_back(c - 1);
        out_vals.push_back(v);
    });

    std::vector<int> expected_rows, expected_cols;
    for (size_t c = 0; c < NC; ++c) {
        for (size_t r = 0; r < NR; ++r) {
            expected_rows.push_back(r);
            expected_cols.push_back(c);
        }
    }

    EXPECT_EQ(out_rows, expected_rows);
    EXPECT_EQ(out_cols, expected_cols);
    EXPECT_EQ(out_vals, values);
}

TEST_P(ParserIntegerBodyTest, ArrayVector) {
    auto chunksize = GetParam();

    size_t N = 1442;
    auto values = simulate_integer(N, -999, 999);

    std::stringstream stored;
    format_array(stored, N, values);
    stored << "\n"; // add an extra newline.
    std::string input = stored.str();

    ChunkedBufferReader reader(input.data(), input.size(), chunksize);
    eminem::Parser parser(reader);
    parser.scan_preamble();

    EXPECT_EQ(parser.get_nrows(), N);
    EXPECT_EQ(parser.get_ncols(), 1);
    EXPECT_EQ(parser.get_nlines(), N);

    std::vector<int> out_rows, out_cols, out_vals;
    parser.scan_integer([&](size_t r, size_t c, int v) -> void {
        out_rows.push_back(r - 1);
        out_cols.push_back(c - 1);
        out_vals.push_back(v);
    });

    std::vector<int> expected_rows(N);
    std::iota(expected_rows.begin(), expected_rows.end(), 0);
    EXPECT_EQ(out_rows, expected_rows);
    EXPECT_EQ(out_cols, std::vector<int>(N));
    EXPECT_EQ(out_vals, values);
}

INSTANTIATE_TEST_CASE_P(
    Parser,
    ParserIntegerBodyTest,
    ::testing::Values(5, 20, 100, 1000)
);

/*****************************************
 *****************************************/

class ParserRealBodyTest : public ::testing::TestWithParam<double> {
protected:
    static void test_equal_vectors(const std::vector<double>& left, const std::vector<double>& right) {
        ASSERT_EQ(left.size(), right.size());
        for (size_t i = 0; i < left.size(); ++i) {
            EXPECT_TRUE(std::abs(left[i] - right[i]) <= 0.00000001);
        }
    }
};

TEST_P(ParserRealBodyTest, CoordinateMatrix) {
    auto chunksize = GetParam();

    size_t NR = 65, NC = 58;
    auto coords = simulate_coordinate(NR, NC, 0.1);
    auto values = simulate_real(coords.first.size());

    std::stringstream stored;
    format_coordinate(stored, NR, NC, coords.first, coords.second, values); // Don't add an extra new line this time!
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
    test_equal_vectors(out_vals, values);

    // Getting some coverage on scan_double in coordinate mode.
    {
        ChunkedBufferReader reader(input.data(), input.size(), chunksize);
        eminem::Parser parser(reader);
        parser.scan_preamble();

        std::vector<double> out_vals2;
        parser.scan_real([&](size_t, size_t, double v) -> void {
            out_vals2.push_back(v);
        });
        EXPECT_EQ(out_vals, out_vals2);
    }
}

TEST_P(ParserRealBodyTest, CoordinateVector) {
    auto chunksize = GetParam();

    size_t N = 6558;
    auto coords = simulate_coordinate(N, 0.05);
    auto values = simulate_real(coords.size());

    std::stringstream stored;
    format_coordinate(stored, N, coords, values);
    stored << "\n"; // inject an extra newline.
    std::string input = stored.str();

    ChunkedBufferReader reader(input.data(), input.size(), chunksize);
    eminem::Parser parser(reader);
    parser.scan_preamble();

    EXPECT_EQ(parser.get_nrows(), N);
    EXPECT_EQ(parser.get_ncols(), 1);
    EXPECT_EQ(parser.get_nlines(), values.size());

    std::vector<int> out_rows, out_cols;
    std::vector<double> out_vals;
    parser.scan_real([&](size_t r, size_t c, double v) -> void {
        out_rows.push_back(r - 1);
        out_cols.push_back(c - 1);
        out_vals.push_back(v);
    });

    EXPECT_EQ(out_rows, coords);
    EXPECT_EQ(out_cols, std::vector<int>(coords.size()));
    test_equal_vectors(out_vals, values);
}

TEST_P(ParserRealBodyTest, ArrayMatrix) {
    auto chunksize = GetParam();

    size_t NR = 93, NC = 85;
    auto values = simulate_real(NR * NC);

    std::stringstream stored;
    format_array(stored, NR, NC, values);
    std::string input = stored.str();

    ChunkedBufferReader reader(input.data(), input.size(), chunksize);
    eminem::Parser parser(reader);
    parser.scan_preamble();

    EXPECT_EQ(parser.get_nrows(), NR);
    EXPECT_EQ(parser.get_ncols(), NC);
    EXPECT_EQ(parser.get_nlines(), NR * NC);

    std::vector<int> out_rows, out_cols;
    std::vector<double> out_vals;
    parser.scan_real([&](size_t r, size_t c, double v) -> void {
        out_rows.push_back(r - 1);
        out_cols.push_back(c - 1);
        out_vals.push_back(v);
    });

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

    // Getting some coverage on scan_double in array mode.
    {
        ChunkedBufferReader reader(input.data(), input.size(), chunksize);
        eminem::Parser parser(reader);
        parser.scan_preamble();

        std::vector<double> out_vals2;
        parser.scan_real([&](size_t, size_t, double v) -> void {
            out_vals2.push_back(v);
        });
        EXPECT_EQ(out_vals, out_vals2);
    }
}

TEST_P(ParserRealBodyTest, ArrayVector) {
    auto chunksize = GetParam();

    size_t N = 632;
    auto values = simulate_real(N);

    std::stringstream stored;
    format_array(stored, N, values);
    std::string input = stored.str();

    ChunkedBufferReader reader(input.data(), input.size(), chunksize);
    eminem::Parser parser(reader);
    parser.scan_preamble();

    EXPECT_EQ(parser.get_nrows(), N);
    EXPECT_EQ(parser.get_ncols(), 1);
    EXPECT_EQ(parser.get_nlines(), N);

    std::vector<int> out_rows, out_cols;
    std::vector<double> out_vals;
    parser.scan_real([&](size_t r, size_t c, double v) -> void {
        out_rows.push_back(r - 1);
        out_cols.push_back(c - 1);
        out_vals.push_back(v);
    });

    std::vector<int> expected_rows(N);
    std::iota(expected_rows.begin(), expected_rows.end(), 0);
    EXPECT_EQ(out_rows, expected_rows);
    EXPECT_EQ(out_cols, std::vector<int>(N));
    test_equal_vectors(out_vals, values);
}

INSTANTIATE_TEST_CASE_P(
    Parser,
    ParserRealBodyTest,
    ::testing::Values(5, 20, 100, 1000)
);

/*****************************************
 *****************************************/

class ParserComplexBodyTest : public ::testing::TestWithParam<double> {
protected:
    static void test_equal_vectors(const std::vector<std::complex<double> >& left, const std::vector<std::complex<double> >& right) {
        ASSERT_EQ(left.size(), right.size());
        for (size_t i = 0; i < left.size(); ++i) {
            EXPECT_TRUE(std::abs(left[i].real() - right[i].real()) <= 0.00000001);
            EXPECT_TRUE(std::abs(left[i].imag() - right[i].imag()) <= 0.00000001);
        }
    }
};

TEST_P(ParserComplexBodyTest, CoordinateMatrix) {
    auto chunksize = GetParam();

    size_t NR = 65, NC = 58;
    auto coords = simulate_coordinate(NR, NC, 0.1);
    auto values = simulate_complex(coords.first.size());

    std::stringstream stored;
    format_coordinate(stored, NR, NC, coords.first, coords.second, values); // Don't add an extra new line this time!
    std::string input = stored.str();

    ChunkedBufferReader reader(input.data(), input.size(), chunksize);
    eminem::Parser parser(reader);
    parser.scan_preamble();

    EXPECT_EQ(parser.get_nrows(), NR);
    EXPECT_EQ(parser.get_ncols(), NC);
    EXPECT_EQ(parser.get_nlines(), values.size());

    std::vector<int> out_rows, out_cols;
    std::vector<std::complex<double> > out_vals;
    parser.scan_complex([&](size_t r, size_t c, std::complex<double> v) -> void {
        out_rows.push_back(r - 1);
        out_cols.push_back(c - 1);
        out_vals.push_back(v);
    });

    EXPECT_EQ(out_rows, coords.first);
    EXPECT_EQ(out_cols, coords.second);
    test_equal_vectors(out_vals, values);
}

TEST_P(ParserComplexBodyTest, CoordinateVector) {
    auto chunksize = GetParam();

    size_t N = 6558;
    auto coords = simulate_coordinate(N, 0.05);
    auto values = simulate_complex(coords.size());

    std::stringstream stored;
    format_coordinate(stored, N, coords, values);
    stored << "\n"; // inject an extra newline.
    std::string input = stored.str();

    ChunkedBufferReader reader(input.data(), input.size(), chunksize);
    eminem::Parser parser(reader);
    parser.scan_preamble();

    EXPECT_EQ(parser.get_nrows(), N);
    EXPECT_EQ(parser.get_ncols(), 1);
    EXPECT_EQ(parser.get_nlines(), values.size());

    std::vector<int> out_rows, out_cols;
    std::vector<std::complex<double> > out_vals;
    parser.scan_complex([&](size_t r, size_t c, std::complex<double> v) -> void {
        out_rows.push_back(r - 1);
        out_cols.push_back(c - 1);
        out_vals.push_back(v);
    });

    EXPECT_EQ(out_rows, coords);
    EXPECT_EQ(out_cols, std::vector<int>(coords.size()));
    test_equal_vectors(out_vals, values);
}

TEST_P(ParserComplexBodyTest, ArrayMatrix) {
    auto chunksize = GetParam();

    size_t NR = 93, NC = 85;
    auto values = simulate_complex(NR * NC);

    std::stringstream stored;
    format_array(stored, NR, NC, values);
    std::string input = stored.str();

    ChunkedBufferReader reader(input.data(), input.size(), chunksize);
    eminem::Parser parser(reader);
    parser.scan_preamble();

    EXPECT_EQ(parser.get_nrows(), NR);
    EXPECT_EQ(parser.get_ncols(), NC);
    EXPECT_EQ(parser.get_nlines(), NR * NC);

    std::vector<int> out_rows, out_cols;
    std::vector<std::complex<double> > out_vals;
    parser.scan_complex([&](size_t r, size_t c, std::complex<double> v) -> void {
        out_rows.push_back(r - 1);
        out_cols.push_back(c - 1);
        out_vals.push_back(v);
    });

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

TEST_P(ParserComplexBodyTest, ArrayVector) {
    auto chunksize = GetParam();

    size_t N = 632;
    auto values = simulate_complex(N);

    std::stringstream stored;
    format_array(stored, N, values);
    std::string input = stored.str();

    ChunkedBufferReader reader(input.data(), input.size(), chunksize);
    eminem::Parser parser(reader);
    parser.scan_preamble();

    EXPECT_EQ(parser.get_nrows(), N);
    EXPECT_EQ(parser.get_ncols(), 1);
    EXPECT_EQ(parser.get_nlines(), N);

    std::vector<int> out_rows, out_cols;
    std::vector<std::complex<double> > out_vals;
    parser.scan_complex([&](size_t r, size_t c, std::complex<double> v) -> void {
        out_rows.push_back(r - 1);
        out_cols.push_back(c - 1);
        out_vals.push_back(v);
    });

    std::vector<int> expected_rows(N);
    std::iota(expected_rows.begin(), expected_rows.end(), 0);
    EXPECT_EQ(out_rows, expected_rows);
    EXPECT_EQ(out_cols, std::vector<int>(N));
    test_equal_vectors(out_vals, values);
}

INSTANTIATE_TEST_CASE_P(
    Parser,
    ParserComplexBodyTest,
    ::testing::Values(5, 20, 100, 1000)
);

/*****************************************
 *****************************************/

class ParserPatternBodyTest : public ::testing::TestWithParam<double> {
protected:
    static void test_equal_vectors(const std::vector<std::complex<double> >& left, const std::vector<std::complex<double> >& right) {
        ASSERT_EQ(left.size(), right.size());
        for (size_t i = 0; i < left.size(); ++i) {
            EXPECT_TRUE(std::abs(left[i].real() - right[i].real()) <= 0.00000001);
            EXPECT_TRUE(std::abs(left[i].imag() - right[i].imag()) <= 0.00000001);
        }
    }
};

TEST_P(ParserPatternBodyTest, CoordinateMatrix) {
    auto chunksize = GetParam();

    size_t NR = 163, NC = 218;
    auto coords = simulate_coordinate(NR, NC, 0.1);

    std::stringstream stored;
    format_coordinate(stored, NR, NC, coords.first, coords.second, std::vector<char>()); // Don't add an extra new line this time!
    std::string input = stored.str();

    ChunkedBufferReader reader(input.data(), input.size(), chunksize);
    eminem::Parser parser(reader);
    parser.scan_preamble();

    EXPECT_EQ(parser.get_nrows(), NR);
    EXPECT_EQ(parser.get_ncols(), NC);
    EXPECT_EQ(parser.get_nlines(), coords.first.size());

    std::vector<int> out_rows, out_cols;
    std::vector<char> out_vals;
    parser.scan_pattern([&](size_t r, size_t c, bool v) -> void {
        out_rows.push_back(r - 1);
        out_cols.push_back(c - 1);
        out_vals.push_back(v);
    });

    EXPECT_EQ(out_rows, coords.first);
    EXPECT_EQ(out_cols, coords.second);
    EXPECT_EQ(out_vals, std::vector<char>(coords.first.size(), 1));
}

TEST_P(ParserPatternBodyTest, CoordinateVector) {
    auto chunksize = GetParam();

    size_t N = 4896;
    auto coords = simulate_coordinate(N, 0.05);

    std::stringstream stored;
    format_coordinate(stored, N, coords, std::vector<char>());
    stored << "\n"; // inject an extra newline.
    std::string input = stored.str();

    ChunkedBufferReader reader(input.data(), input.size(), chunksize);
    eminem::Parser parser(reader);
    parser.scan_preamble();

    EXPECT_EQ(parser.get_nrows(), N);
    EXPECT_EQ(parser.get_ncols(), 1);
    EXPECT_EQ(parser.get_nlines(), coords.size());

    std::vector<int> out_rows, out_cols;
    std::vector<char> out_vals;
    parser.scan_pattern([&](size_t r, size_t c, bool v) -> void {
        out_rows.push_back(r - 1);
        out_cols.push_back(c - 1);
        out_vals.push_back(v);
    });

    EXPECT_EQ(out_rows, coords);
    EXPECT_EQ(out_cols, std::vector<int>(coords.size()));
    EXPECT_EQ(out_vals, std::vector<char>(coords.size(), 1));
}

INSTANTIATE_TEST_CASE_P(
    Parser,
    ParserPatternBodyTest,
    ::testing::Values(5, 20, 100, 1000)
);

/*****************************************
 *****************************************/

class ParserBodyErrorTest : public ::testing::Test {
protected:
    void test_error(const std::string& input, std::string msg) {
        EXPECT_ANY_THROW({
            try {
                ChunkedBufferReader reader(input.data(), input.size(), 20);
                eminem::Parser parser(reader);
                parser.scan_preamble();

                auto field = parser.get_banner().field;
                if (field == eminem::Field::INTEGER) {
                    parser.scan_integer([&](size_t, size_t, int){});
                } else if (field == eminem::Field::REAL) {
                    parser.scan_real([&](size_t, size_t, double){});
                } else if (field == eminem::Field::COMPLEX) {
                    parser.scan_complex([&](size_t, size_t, std::complex<double>){});
                } else if (field == eminem::Field::PATTERN) {
                    parser.scan_pattern([&](size_t, size_t, bool){});
                }
            } catch (std::exception& e) {
                EXPECT_THAT(e.what(), ::testing::HasSubstr(msg));
                throw;
            }
        });
    }
};

TEST_F(ParserBodyErrorTest, CoordinateErrors) {
    test_error("%MatrixMarket matrix coordinate integer general\n5 2 1\n1\n", "expected 3 fields");
    test_error("%MatrixMarket vector coordinate integer general\n5 2 1\n1\n", "expected 2 fields");
    test_error("%MatrixMarket matrix coordinate integer general\n5 2 1\n1  2\n", "detected empty field");
    test_error("%MatrixMarket matrix coordinate integer general\n5 2 1\n1 2 \n", "empty field detected");

    test_error("%MatrixMarket matrix coordinate integer general\n5 2 1\n0 2 1\n", "row index must be positive");
    test_error("%MatrixMarket matrix coordinate integer general\n5 2 1\n6 2 1\n", "row index out of range");
    test_error("%MatrixMarket matrix coordinate integer general\n5 2 1\nasd 2 1\n", "row index should be a non-negative integer");

    test_error("%MatrixMarket matrix coordinate integer general\n5 2 1\n2 0 1\n", "column index must be positive");
    test_error("%MatrixMarket matrix coordinate integer general\n5 2 1\n2 5 1\n", "column index out of range");
    test_error("%MatrixMarket matrix coordinate integer general\n5 2 1\n5 -5 1\n", "column index should be a non-negative integer");

    test_error("%MatrixMarket matrix coordinate integer general\n5 2 1\n2 1 1\n3 2 4\n", "more lines present than specified");
    test_error("%MatrixMarket matrix coordinate integer general\n5 2 1\n", "fewer lines present than specified");
}

TEST_F(ParserBodyErrorTest, ArrayErrors) {
    test_error("%MatrixMarket matrix array integer general\n5 2\n1 2\n", "expected 1 field");
    test_error("%MatrixMarket matrix array integer general\n5 2\n\n", "empty field detected");
    test_error("%MatrixMarket matrix array integer general\n5 2\n2\n3\n", "fewer lines present than expected");
    test_error("%MatrixMarket matrix array integer general\n1 1\n2\n2\n", "more lines present than expected");
}

TEST_F(ParserBodyErrorTest, IntegerErrors) {
    test_error("%MatrixMarket matrix coordinate integer general\n5 2 1\n1 2 -5.0\n", "integer value");
}

TEST_F(ParserBodyErrorTest, RealErrors) {
    test_error("%MatrixMarket matrix coordinate real general\n5 2 1\n1 2 asdasd\n", "failed to convert");
    test_error("%MatrixMarket matrix coordinate real general\n5 2 1\n1 2 127.0.0.1\n", "failed to convert");
}

TEST_F(ParserBodyErrorTest, ComplexErrors) {
    test_error("%MatrixMarket matrix coordinate complex general\n5 2 1\n1 2 asdasd 23\n", "failed to convert");
    test_error("%MatrixMarket matrix coordinate complex general\n5 2 1\n1 2 123 127.0.0.1\n", "failed to convert");
    test_error("%MatrixMarket matrix coordinate complex general\n5 2 1\n1 2 123\n", "expected 4");
    test_error("%MatrixMarket vector coordinate complex general\n5 1\n1 2 123 42\n", "expected 3");
}

TEST_F(ParserBodyErrorTest, PatternErrors) {
    test_error("%MatrixMarket matrix coordinate pattern general\n5 2 1\n1 2 123\n", "expected 2");
    test_error("%MatrixMarket vector coordinate pattern general\n5 1\n1 2\n", "expected 1");
    test_error("%MatrixMarket matrix array pattern general\n5 1\n1 2\n", "not supported");
}


