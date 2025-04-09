#include <gtest/gtest.h>

#include "eminem/from_text.hpp"
#include "eminem/from_gzip.hpp"

#include "temp_file_path.h"
#include "simulate.h"
#include "format.h"

#include <limits>
#include <fstream>
#include <string>
#include <vector>
#include <random>

TEST(FromGzip, File) {
    size_t NR = 192, NC = 132;
    auto coords = simulate_coordinate(NR, NC, 0.1);
    auto values = simulate_integer(coords.first.size(), -999, 999);

    auto path = temp_file_path("gzipped");
    {
        std::stringstream stored;
        format_coordinate(stored, NR, NC, coords.first, coords.second, values);
        stored << "\n"; // add an extra newline.
        auto contents = stored.str();

        gzFile ohandle = gzopen(path.c_str(), "w");
        gzwrite(ohandle, contents.data(), contents.size());
        gzclose(ohandle);
    }

    // Works with the Gzip parser.
    {
        eminem::GzipFileParser parser(path.c_str(), {});

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

    // Works with the Some* parser.
    {
        eminem::SomeFileParser parser(path.c_str(), {});

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

TEST(FromGzip, Buffer) {
    size_t NR = 112, NC = 152;
    auto coords = simulate_coordinate(NR, NC, 0.1);
    auto values = simulate_integer(coords.first.size(), -999, 999);

    std::vector<unsigned char> gzcontents;
    {
        std::stringstream stored;
        format_coordinate(stored, NR, NC, coords.first, coords.second, values);
        stored << "\n"; // add an extra newline.
        auto contents = stored.str();

        auto path = temp_file_path("gzipped_buffer");
        gzFile ohandle = gzopen(path.c_str(), "w");
        gzwrite(ohandle, contents.data(), contents.size());
        gzclose(ohandle);

        std::ifstream handle(path, std::ios_base::binary);
        handle >> std::noskipws;
        gzcontents.insert(gzcontents.end(), std::istream_iterator<unsigned char>{handle}, std::istream_iterator<unsigned char>());
    }

    // Works with the Zlib parser.
    {
        eminem::ZlibBufferParser parser(gzcontents.data(), gzcontents.size(), {});

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

    // Works with the Some* parser.
    {
        eminem::SomeBufferParser parser(gzcontents.data(), gzcontents.size(), {});

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
