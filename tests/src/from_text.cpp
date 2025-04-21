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

class FromTextTest : public ::testing::TestWithParam<std::tuple<int, int> > {};

TEST_P(FromTextTest, File) {
    std::size_t NR = 192, NC = 132;
    auto coords = simulate_coordinate(NR, NC, 0.1);
    auto values = simulate_integer(coords.first.size(), -999, 999);

    auto path = temp_file_path("text");
    {
        std::ofstream stored(path);
        format_coordinate(stored, NR, NC, coords.first, coords.second, values);
        stored << "\n"; // add an extra newline.
    }

    auto param = GetParam();
    auto nthreads = std::get<0>(param);
    auto blocksize = std::get<1>(param);

    // Works with the text file parser.
    {
        eminem::ParseTextFileOptions opt;
        opt.num_threads = nthreads;
        opt.buffer_size = blocksize;
        opt.block_size = blocksize;
        auto parser = eminem::parse_text_file(path.c_str(), opt);

        parser.scan_preamble();
        EXPECT_EQ(parser.get_nrows(), NR);
        EXPECT_EQ(parser.get_ncols(), NC);
        EXPECT_EQ(parser.get_nlines(), values.size());

        std::vector<int> out_rows, out_cols, out_vals;
        parser.scan_integer([&](eminem::Index r, eminem::Index c, int v) -> void {
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
        eminem::ParseSomeFileOptions opt;
        opt.num_threads = nthreads;
        opt.buffer_size = blocksize;
        opt.block_size = blocksize;
        auto parser = eminem::parse_some_file(path.c_str(), opt);

        parser.scan_preamble();
        EXPECT_EQ(parser.get_nrows(), NR);
        EXPECT_EQ(parser.get_ncols(), NC);
        EXPECT_EQ(parser.get_nlines(), values.size());

        std::vector<int> out_rows, out_cols, out_vals;
        parser.scan_integer([&](eminem::Index r, eminem::Index c, int v) -> void {
            out_rows.push_back(r - 1);
            out_cols.push_back(c - 1);
            out_vals.push_back(v);
        });

        EXPECT_EQ(out_rows, coords.first);
        EXPECT_EQ(out_cols, coords.second);
        EXPECT_EQ(out_vals, values);
    }
}

TEST_P(FromTextTest, Buffer) {
    std::size_t NR = 112, NC = 152;
    auto coords = simulate_coordinate(NR, NC, 0.1);
    auto values = simulate_integer(coords.first.size(), -999, 999);

    std::stringstream stored;
    format_coordinate(stored, NR, NC, coords.first, coords.second, values);
    stored << "\n"; // add an extra newline.
    auto input = stored.str();

    auto param = GetParam();
    auto nthreads = std::get<0>(param);
    auto blocksize = std::get<1>(param);

    // Works with the test buffer parser.
    {
        eminem::ParseTextBufferOptions opt;
        opt.num_threads = nthreads;
        opt.block_size = blocksize;
        auto parser = eminem::parse_text_buffer(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), opt);

        parser.scan_preamble();
        EXPECT_EQ(parser.get_nrows(), NR);
        EXPECT_EQ(parser.get_ncols(), NC);
        EXPECT_EQ(parser.get_nlines(), values.size());

        std::vector<int> out_rows, out_cols, out_vals;
        parser.scan_integer([&](eminem::Index r, eminem::Index c, int v) -> void {
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
        eminem::ParseSomeBufferOptions opt;
        opt.num_threads = nthreads;
        opt.buffer_size = blocksize;
        opt.block_size = blocksize;
        auto parser = eminem::parse_some_buffer(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), opt);

        parser.scan_preamble();
        EXPECT_EQ(parser.get_nrows(), NR);
        EXPECT_EQ(parser.get_ncols(), NC);
        EXPECT_EQ(parser.get_nlines(), values.size());

        std::vector<int> out_rows, out_cols, out_vals;
        parser.scan_integer([&](eminem::Index r, eminem::Index c, int v) -> void {
            out_rows.push_back(r - 1);
            out_cols.push_back(c - 1);
            out_vals.push_back(v);
        });

        EXPECT_EQ(out_rows, coords.first);
        EXPECT_EQ(out_cols, coords.second);
        EXPECT_EQ(out_vals, values);
    }
}

INSTANTIATE_TEST_SUITE_P(
    FromText,
    FromTextTest,
    ::testing::Values(
        std::tuple<int, int>(1, 10000),
        std::tuple<int, int>(2, 5000),
        std::tuple<int, int>(3, 1000)
    )
);
