#include "eminem/eminem.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << argv[0] << " [PATH TO MM FILE]" << std::endl;
        return 1;
    }

    eminem::SomeFileParser<false> parser(argv[1]);
    parser.scan_preamble();
    auto banner = parser.get_banner();

    std::vector<int> row_count(parser.get_nrows());
    std::vector<int> col_count(parser.get_ncols());
    double total = 0;

    if (banner.field == eminem::Field::INTEGER) {
        parser.scan_integer([&](size_t r, size_t c, int v) -> void {
            ++(row_count[r]);
            ++(col_count[c]);
            total += v;
        });

    } else if (banner.field == eminem::Field::REAL || banner.field == eminem::Field::DOUBLE) {
        parser.scan_real([&](size_t r, size_t c, double v) -> void {
            ++(row_count[r]);
            ++(col_count[c]);
            total += v;
        });

    } else {
        std::cerr << "unsupported matrix type" << std::endl;
        return 1;
    }

    // Counting the number of entries.
    size_t nzrow = 0;
    for (auto x : row_count) {
        nzrow += (x > 0);
    }

    size_t nzcol = 0;
    for (auto x : col_count) {
        nzcol += (x > 0);
    }

    std::cout << parser.get_nrows() << " total rows, " << nzrow << " non-empty rows" << std::endl;
    std::cout << parser.get_ncols() << " total columns, " << nzcol << " non-empty columns" << std::endl;
    std::cout << parser.get_nlines() << " total lines, " << total << " total value" << std::endl;
    return 0;
}
