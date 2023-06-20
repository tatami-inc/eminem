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

TEST(FromText, File) {
    size_t NR = 192, NC = 132;
    auto coords = simulate_coordinate(NR, NC, 0.1);
    auto values = simulate_integer(coords.first.size(), -999, 999);

    auto path = temp_file_path("tatami-tests-ext-MatrixMarket.mtx");
    {
        std::ofstream stored(path);
        format_coordinate(stored, NR, NC, coords.first, coords.second, values);
        stored << "\n"; // add an extra newline.
    }

    // Works with the text file parser.
    {
        eminem::TextFileParser parser(path.c_str());

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
        eminem::SomeFileParser parser(path.c_str());

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

TEST(FromText, Buffer) {
    size_t NR = 112, NC = 152;
    auto coords = simulate_coordinate(NR, NC, 0.1);
    auto values = simulate_integer(coords.first.size(), -999, 999);

    std::stringstream stored;
    format_coordinate(stored, NR, NC, coords.first, coords.second, values);
    stored << "\n"; // add an extra newline.
    auto input = stored.str();

    // Works with the text buffer parser.
    {
        eminem::TextBufferParser parser(input.c_str(), input.size());

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

    // Works with the some buffer parser.
    {
        eminem::SomeBufferParser parser(reinterpret_cast<const unsigned char*>(input.c_str()), input.size());

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
