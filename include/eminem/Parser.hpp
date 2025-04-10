#ifndef EMINEM_PARSER_HPP
#define EMINEM_PARSER_HPP

#include <vector>
#include <string>
#include <cstring>
#include <complex>
#include <type_traits>
#include <memory>

#include "utils.hpp"

/**
 * @file Parser.hpp
 *
 * @brief Parse a matrix from a Matrix Market file.
 */

namespace eminem {

/* GENERAL COMMENTS:
 * - This sticks to the specification described at https://math.nist.gov/MatrixMarket/reports/MMformat.ps.gz
 * - We will allow both tabs and whitespaces when considering a 'blank' character.
 * - We must consider the possibility that the final line of the file is not newline terminated.
 */

/**
 * @brief Parse a matrix from a Matrix Market file.
 *
 * @tparam Input_ Class for the source of input bytes, satisfying the `byteme::PerByteInterface` instance.
 */
template<class Input_>
class Parser {
public:
    /**
     * @param input Source of input bytes, typically a `byteme::PerByteInterface` instance.
     */
    Parser(std::unique_ptr<Input_> input) : my_input(std::move(input)) {}

private:
    std::unique_ptr<Input_> my_input;
    size_t my_current_line = 0;
    MatrixDetails my_details;

    bool chomp() {
        while (1) {
            char x = my_input->get();
            if (x != ' ' && x != '\t') {
                return true;
            }
            if (!(my_input->advance())) {
                break;
            }
        }
        return false;
    }

    // Skip comments and empty lines.
    bool skip_lines() {
        while (1) {
            char x = my_input->get();
            if (x == '%') {
                do {
                    if (!(my_input->advance())) {
                        return false;
                    }
                } while (my_input->get() != '\n');
            } else if (x != '\n') {
                break;
            }

            if (!my_input->advance()) { // move past the newline.
                return false;
            }
            ++my_current_line;
        }
        return true;
    }

private:
    bool my_passed_banner = false;

    struct ExpectedMatch {
        ExpectedMatch(bool found, bool newline, bool remaining) : found(found), newline(newline), remaining(remaining) {}
        ExpectedMatch() : ExpectedMatch(false, false, false) {}
        bool found;
        bool on_newline;
        bool remaining;
    };

    ExpectedMatch advance_past_expected_string() {
        bool remaining = my_input->advance(); // move off the last character.
        ExpectedMatch output(true, false, remaining);

        if (output.remaining) {
            char next = my_input->get();
            if (next == ' ' || next == '\t') {
                if (my_input->advance()) {
                    output.remaining = chomp(); // gobble up all of the remaining horizontal space.
                    if (output.remaining) {
                        output.on_newline = (my_input->get() == '\n');
                    }
                } else {
                    output.remaining = false; 
                }
            } else if (next == '\n') {
                output.on_newline = true; // don't move past the newline.
            } else {
                output.found = false; // if the next character is not a space or whitespace, it's not a match.
            }
        }

        return output;
    }

    ExpectedMatch is_expected_string(const char* ptr, size_t len, size_t start) {
        // It is assumed that the first 'start' characters of 'ptr' where
        // already checked and matched before entering this function, and that
        // 'my_input' is currently positioned at the start-th character, i.e.,
        // 'ptr[start-1]' (and thus requires an advance() call before we can
        // compare against 'ptr[start]').
        for (size_t i = start; i < len; ++i) {
            if (!my_input->advance()) {
                return ExpectedMatch(false, false, false);
            }
            if (my_input->get() != ptr[i]) {
                return ExpectedMatch(false, false, true);
            }
        }
        return advance_past_expected_string();
    }

    ExpectedMatch is_expected_string(const char* ptr, size_t len) {
        // Using a default start of 1, assuming that we've already compared
        // the first character before entering this function.
        return is_expected_string(ptr, len, 1);
    }

    bool parse_banner_object() {
        ExpectedMatch res;

        char x = my_input->get();
        if (x == 'm') {
            res = is_expected_string("matrix", 6);
            my_details.object = Object::MATRIX;
        } else if (x == 'v') {
            res = is_expected_string("vector", 6);
            my_details.object = Object::VECTOR;
        }

        if (!res.found) {
            throw std::runtime_error("first banner field should be one of 'matrix' or 'vector'");
        }
        if (!res.remaining) {
            throw std::runtime_error("end of file is reached after the first banner field");
        }

        return res.on_newline;
    }

    bool parse_banner_format() {
        ExpectedMatch res;

        char x = my_input->get();
        if (x == 'c') {
            res = is_expected_string("coordinate", 10);
            my_details.format = Format::COORDINATE;
        } else if (x == 'a') {
            res = is_expected_string("array", 5);
            my_details.format = Format::ARRAY;
        }

        if (!res.found) {
            throw std::runtime_error("second banner field should be one of 'coordinate' or 'array'");
        }
        if (!res.remaining) {
            throw std::runtime_error("end of file is reached after the second banner field");
        }

        return res.on_newline;
    }

    bool parse_banner_field() {
        ExpectedMatch res;

        char x = my_input->get();
        if (x == 'i') { 
            res = is_expected_string("integer", 7);
            my_details.field = Field::INTEGER;
        } else if (x == 'd') { 
            res = is_expected_string("double", 6);
            my_details.field = Field::DOUBLE;
        } else if (x == 'c') {
            res = is_expected_string("complex", 7);
            my_details.field = Field::COMPLEX;
        } else if (x == 'p') {
            res = is_expected_string("pattern", 7);
            my_details.field = Field::PATTERN;
        } else if (x == 'r') {
            res = is_expected_string("real", 4);
            my_details.field = Field::REAL;
        }

        if (!res.found) {
            throw std::runtime_error("third banner field should be one of 'real', 'integer', 'double', 'complex' or 'pattern'");
        }
        if (!res.remaining) {
            throw std::runtime_error("end of file reached after the third banner field");
        }

        return res.on_newline;
    }

    bool parse_banner_symmetry() {
        ExpectedMatch res;

        char x = my_input->get();
        if (x == 'g') {
            res = is_expected_string("general", 7);
            my_details.symmetry = Symmetry::GENERAL;
        } else if (x == 'h') {
            res = is_expected_string("hermitian", 9);
            my_details.symmetry = Symmetry::HERMITIAN;
        } else if (x == 's') {
            if (my_input->advance()) {
                char x = my_input->get();
                if (x == 'k') {
                    res = is_expected_string("skew-symmetric", 14, 2);
                    my_details.symmetry = Symmetry::SKEW_SYMMETRIC;
                } else {
                    res = is_expected_string("symmetric", 9, 2);
                    my_details.symmetry = Symmetry::SYMMETRIC;
                }
            }
        }

        if (!res.found) {
            throw std::runtime_error("fourth banner field should be one of 'general', 'hermitian', 'skew-symmetric' or 'symmetric'");
        }
        if (!res.remaining) {
            throw std::runtime_error("end of file reached after the fourth banner field");
        }

        return res.on_newline;
    }

    void scan_banner() {
        if (my_passed_banner) {
            throw std::runtime_error("banner has already been scanned");
        }
        if (!(my_input->valid())) {
            throw std::runtime_error("failed to find banner line before end of file");
        }
        if (my_input->get() != '%') {
            throw std::runtime_error("first line of the file should be the banner");
        }

        auto found_banner = is_expected_string("%%MatrixMarket", 14);
        if (!found_banner.remaining) {
            throw std::runtime_error("end of file reached before matching the banner");
        }
        if (!found_banner.found) {
            throw std::runtime_error("first line of the file should be the banner");
        }
        if (found_banner.on_newline) {
            throw std::runtime_error("end of line reached before the first banner field");
        }

        if (parse_banner_object()) {
            throw std::runtime_error("end of line is reached after the first banner field");
        }
        if (parse_banner_format()) {
            throw std::runtime_error("end of line is reached after the second banner field");
        }

        bool eol = false;
        if (my_details.object == Object::MATRIX) {
            if (parse_banner_field()) {
                throw std::runtime_error("end of line is reached after the third banner field");
            }
            eol = parse_banner_symmetry();
        } else {
            // The NIST spec doesn't say anything about symmetry for vector,
            // and it doesn't really make sense anyway. We'll just set it to
            // general and hope for the best.
            my_details.symmetry = Symmetry::GENERAL;

            // No need to throw on newline because this might be the last field AFAICT.
            eol = parse_banner_field();
        }

        my_passed_banner = true;

        // Ignoring all other fields until the newline. We can use a do/while
        // to skip a comparison because we know that the current character
        // cannot be a newline if eol = false.
        if (!eol) {
            do {
                if (!(my_input->advance())) {
                    throw std::runtime_error("end of file reached before the end of the banner line");
                }
            } while (my_input->get() != '\n');
        }
        my_input->advance(); // advance past the newline.
        ++my_current_line;
        return;
    }

public:
    /**
     * Retrieve the Matrix Market banner, containing information about the data format and type.
     * This should only be called after `scan_preamble()`.
     *
     * @return Details about the matrix in this file.
     */
    const MatrixDetails& get_banner() const {
        if (!my_passed_banner) {
            throw std::runtime_error("banner has not yet been scanned");
        }
        return my_details;
    }

    struct SizeInfo {
        size_t index = 0;
        bool remaining = false;
    };

    template<bool terminal_>
    SizeInfo scan_size_field() {
        SizeInfo output;
        bool found = false;

        while (1) {
            char x = my_input->get();
            switch(x) {
                case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                    found = true;
                    output.index *= 10;
                    output.index += x - '0';
                    break;
                case '\n':
                    if constexpr(terminal_) {
                        output.remaining = my_input->advance(); // advance past the newline.
                        return output;
                    }
                    throw std::runtime_error("unexpected newline when parsing size field on line " + std::to_string(my_current_line + 1));
                case ' ': case '\t':
                    if (!(my_input->advance())) {
                        throw std::runtime_error("premature termination of a size field on line " + std::to_string(my_current_line + 1));
                    }
                    if (!chomp()) {
                        throw std::runtime_error("premature termination of a size field on line " + std::to_string(my_current_line + 1));
                    }
                    if constexpr(terminal_) {
                        if (my_input->get() != '\n') {
                            throw std::runtime_error("failed to find a newline after the last size field on line " + std::to_string(my_current_line + 1));
                        }
                        output.remaining = my_input->advance(); // advance past the newline.
                    } else {
                        output.remaining = true;
                    }
                    return output;
                default:
                    throw std::runtime_error("unexpected character when parsing size field on line " + std::to_string(my_current_line + 1));
            }

            if (!(my_input->advance())) {
                if constexpr(terminal_) {
                    break;
                } else {
                    throw std::runtime_error("premature termination of a size field on line " + std::to_string(my_current_line + 1));
                }
            }
        }

        if (!found) {
            throw std::runtime_error("detected an empty size field on line " + std::to_string(my_current_line + 1));
        }
        return output;
    }

private:
    bool my_passed_size = false;
    size_t my_nrows = 0, my_ncols = 0, my_nlines = 0;

    void scan_size() {
        if (!(my_input->valid())) {
            throw std::runtime_error("failed to find size line before end of file");
        }

        // Handling stray comments, empty lines, and leading whitespace.
        if (!skip_lines()) {
            throw std::runtime_error("failed to find size line before end of file");
        }
        if (!chomp()) {
            throw std::runtime_error("expected at least one size field on line " + std::to_string(my_current_line + 1));
        }

        if (my_details.object == Object::MATRIX) {
            if (my_details.format == Format::COORDINATE) {
                auto first_field = scan_size_field<false>();
                if (!first_field.remaining) {
                    throw std::runtime_error("expected three size fields for coordinate matrices on line " + std::to_string(my_current_line + 1));
                }
                my_nrows = first_field.index;

                auto second_field = scan_size_field<false>();
                if (!second_field.remaining) {
                    throw std::runtime_error("expected three size fields for coordinate matrices on line " + std::to_string(my_current_line + 1));
                }
                my_ncols = second_field.index;

                auto third_field = scan_size_field<true>();
                my_nlines = third_field.index;

            } else { // i.e., my_details.format == Format::ARRAY
                auto first_field = scan_size_field<false>();
                if (!first_field.remaining) {
                    throw std::runtime_error("expected two size fields for array matrices on line " + std::to_string(my_current_line + 1));
                }
                my_nrows = first_field.index;

                auto second_field = scan_size_field<true>();
                my_ncols = second_field.index;
                my_nlines = my_nrows * my_ncols;
            }

        } else {
            if (my_details.format == Format::COORDINATE) {
                auto first_field = scan_size_field<false>();
                if (!first_field.remaining) {
                    throw std::runtime_error("expected two size fields for coordinate vectors on line " + std::to_string(my_current_line + 1));
                }
                my_nrows = first_field.index;

                auto second_field = scan_size_field<true>();
                my_nlines = second_field.index;

            } else { // i.e., my_details.format == Format::ARRAY
                auto first_field = scan_size_field<true>();
                my_nlines = first_field.index;
                my_nrows = my_nlines;
            }
            my_ncols = 1;
        }

        my_passed_size = true;
    }

public:
    /**
     * Get the number of rows in the matrix.
     * This should only be called after `scan_preamble()`.
     * If the object type is `Object::VECTOR`, the number of rows is equal to the length of the vector.
     *
     * @return Number of rows.
     */
    size_t get_nrows() const {
        if (!my_passed_size) {
            throw std::runtime_error("size line has not yet been scanned");
        }
        return my_nrows;
    }

    /**
     * Get the number of columns in the matrix.
     * This should only be called after `scan_preamble()`.
     * If the object type is `Object::VECTOR`, the number of columns is set to 1.
     *
     * @return Number of columns.
     */
    size_t get_ncols() const {
        if (!my_passed_size) {
            throw std::runtime_error("size line has not yet been scanned");
        }
        return my_ncols;
    }

    /**
     * Get the number of non-zero lines in the coordinate format.
     * This should only be called after `scan_preamble()`.
     * If the object type is `Object::ARRAY`, the number of lines is defined as the product of the number of rows and columns.
     *
     * @return Number of non-zero lines. 
     */
    size_t get_nlines() const {
        if (!my_passed_size) {
            throw std::runtime_error("size line has not yet been scanned");
        }
        return my_nlines;
    }

public:
    /**
     * Scan the preamble from the Matrix Market file, including the banner and the size line.
     * This should only be called once.
     */
    void scan_preamble() {
        scan_banner();
        scan_size();
        return;
    }

private:
    static void check_matrix_coordinate_line(size_t current_data_line, size_t currow, size_t curcol) {
        if (current_data_line >= my_nlines) {
            throw std::runtime_error("more lines present than specified in the header (" + std::to_string(my_nlines) + ")");
        }
        if (!currow) {
            throw std::runtime_error("row index must be positive on line " + std::to_string(my_current_line + 1));
        }
        if (currow > my_nrows) {
            throw std::runtime_error("row index out of range on line " + std::to_string(my_current_line + 1));
        }
        if (!curcol) {
            throw std::runtime_error("column index must be positive on line " + std::to_string(my_current_line + 1));
        }
        if (curcol > my_ncols) {
            throw std::runtime_error("column index out of range on line " + std::to_string(my_current_line + 1));
        }
    }

    struct StoreInfo {
        bool quit_early = false;
        bool remaining = false;
    };

    template<class Store_>
    bool scan_matrix_coordinate_non_pattern(Store_ store) {
        if (!my_passed_banner || !my_passed_size) {
            throw std::runtime_error("banner or size lines have not yet been parsed");
        }

        size_t current_data_line = 0;
        bool valid = my_input->valid();
        while (valid) {
            // Handling stray comments, empty lines, and leading spaces.
            if (!skip_lines()) {
                break;
            }
            if (!chomp()) {
                break;
            }

            auto first_field = scan_size_field<false>();
            if (!first_field.remaining) {
                throw std::runtime_error("expected two size fields for a coordinate matrix on line " + std::to_string(my_current_line + 1));
            }
            auto second_field = scan_size_field<false>();
            if (!second_field.remaining) {
                throw std::runtime_error("expected at least three fields for a coordinate matrix on line " + std::to_string(my_current_line + 1));
            }
            check_matrix_coordinate_line(current_data_line, first_field.index, second_field.index);

            // 'store' should leave 'my_input' in the position immediately
            // after the successfully parsed value; this may or may not be EOL.
            auto res = store(first_field.first, second_field.first);
            if (res.quit_early) {
                return true;
            }
            ++current_data_line;
            ++my_current_line;
            if (!res.remaining) { // if it doesn't end on a newline, fine, we'll quit.
                break;
            }

            // Cleaning up blanks in the rest of the line.
            }
            valid = input->advance(); // move past the newline.
        }

        if (current_data_line != my_nlines) {
            throw std::runtime_error("observed and expected number of lines are different for a coordinate matrix (" + std::to_string(my_nlines) + ")");
        }
        return false;
    }

    template<class PatternStore_>
    bool scan_matrix_coordinate_pattern(PatternStore_ pstore) {
        if (!my_passed_banner || !my_passed_size) {
            throw std::runtime_error("banner or size lines have not yet been parsed");
        }

        size_t current_data_line = 0;
        if (my_input->valid()) {
            while (1) {
                // Handling stray comments, empty lines, and leading spaces.
                if (!skip_lines()) {
                    break;
                }
                if (!chomp()) {
                    break;
                }

                std::pair<size_t, bool> first_field = scan_size_field<false>();
                if (!first_field.second) {
                    throw std::runtime_error("expected two size fields for a coordinate matrix on line " + std::to_string(my_current_line + 1));
                }
                std::pair<size_t, bool> second_field = scan_size_field<true>();
                check_matrix_coordinate_line(current_data_line, first_field.first, second_field.first);

                // 'pstore' returns boolean indicating whether to quit early;
                // it doesn't modify 'my_input' at all.
                if (pstore(first_field.first, second_field.first, true)) {
                    return true;
                }
                ++current_data_line;
                ++my_current_line;
                if (!second_field.second) { // if it doesn't end on a newline, fine, we'll quit.
                    break;
                }
            }
        }

        if (current_data_line != my_nlines) {
            throw std::runtime_error("observed and expected number of lines are different for a coordinate matrix (" + std::to_string(my_nlines) + ")");
        }
        return false;
    }

public:
    static void check_vector_coordinate_line(size_t current_data_line, size_t currow) {
        if (current_data_line >= my_nlines) {
            throw std::runtime_error("more lines present than specified in the header (" + std::to_string(my_nlines) + ")");
        }
        if (!currow) {
            throw std::runtime_error("row index must be positive on line " + std::to_string(my_current_line + 1));
        }
        if (currow > my_nrows) {
            throw std::runtime_error("row index out of range on line " + std::to_string(my_current_line + 1));
        }
    }

    template<class Store_>
    bool scan_vector_coordinate_non_pattern(Store_ store) {
        if (!my_passed_banner || !my_passed_size) {
            throw std::runtime_error("banner or size lines have not yet been parsed");
        }

        size_t current_data_line = 0;
        bool valid = my_input->valid();
        while (valid) {
            // Handling stray comments, empty lines, and leading spaces.
            if (!skip_lines()) {
                break;
            }
            if (!chomp()) {
                break;
            }

            std::pair<size_t, bool> first_field = scan_size_field<false>();
            if (!first_field.second) {
                throw std::runtime_error("expected at least two fields for a coordinate vector on line " + std::to_string(my_current_line + 1));
            }
            check_vector_coordinate_line(current_data_line, first_field.first);

            // 'store' should leave 'my_input' in the position immediately
            // after the successfully parsed value; this may or may not be EOL.
            auto res = store(first_field.first, 1);
            if (res.quit_early) {
                return true;
            }
            ++current_data_line;
            ++my_current_line;
            if (!res.remaining) { // if it doesn't end on a newline, fine, we'll quit.
                break;
            }

            // Cleaning up blanks in the rest of the line.
            if (!res.on_newline) {
                if (!chomp()) {
                    break;
                }
                if (my_input->get() != '\n') {
                    throw std::runtime_error("more fields than expected for a coordinate vector on line " + std::to_string(my_current_line + 1));
                }
            }
            valid = input->advance(); // move past the newline.
        }

        if (current_data_line != my_nlines) {
            throw std::runtime_error("observed and expected number of lines are different for a coordinate vector (" + std::to_string(my_nlines) + ")");
        }
        return false;
    }

    template<class PatternStore_>
    bool scan_vector_coordinate_pattern(PatternStore_ pstore) {
        if (!my_passed_banner || !my_passed_size) {
            throw std::runtime_error("banner or size lines have not yet been parsed");
        }

        size_t current_data_line = 0;
        if (my_input->valid()) {
            while (1) {
                // Handling stray comments, empty lines, and leading spaces.
                if (!skip_lines()) {
                    break;
                }
                if (!chomp()) {
                    break;
                }

                std::pair<size_t, bool> first_field = scan_size_field<true>();
                check_vector_coordinate_line(current_data_line, first_field.first);

                // 'pstore' returns boolean indicating whether to quit early;
                // it doesn't modify 'my_input' at all.
                if (pstore(first_field.first, 1, true)) {
                    return true;
                }
                ++current_data_line;
                ++my_current_line;
                if (!first_field.second) { // if it doesn't end on a newline, fine, we'll quit.
                    break;
                }
            }
        }

        if (current_data_line != my_nlines) {
            throw std::runtime_error("observed and expected number of lines are different for a coordinate vector (" + std::to_string(my_nlines) + ")");
        }
        return false;
    }

private:
    template<class Store_>
    bool scan_matrix_array(Store_ store) {
        if (!my_passed_banner || !my_passed_size) {
            throw std::runtime_error("banner or size lines have not yet been parsed");
        }

        size_t current_data_line = 0;
        bool valid = my_input->valid();
        while (valid) {
            // Handling stray comments, empty lines, and leading spaces.
            if (!skip_lines()) {
                break;
            }
            if (!chomp()) {
                break;
            }

            if (current_data_line >= my_nlines) {
                throw std::runtime_error("more lines present than expected for an array format (" + std::to_string(my_nlines) + ")");
            }

            // 'store' should leave 'my_input' in the position immediately
            // after the successfully parsed value; this may or may not be EOL.
            auto res = store(currow, curcol);
            if (res.quit_early) {
                return true;
            }
            ++current_data_line;
            ++currow;
            if (currow > my_nrows) {
                ++curcol;
                currow = 1;
            }
            if (!res.remaining) {
                break;
            }

            // Cleaning up blanks in the rest of the line.
            if (!res.on_newline) {
                if (!chomp()) {
                    break;
                }
                if (my_input->get() != '\n') {
                    throw std::runtime_error("more fields than expected for a coordinate matrix on line " + std::to_string(my_current_line + 1));
                }
            }
            valid = my_input->advance(); // move past the newline.
        }

        if (current_data_line != my_nlines) {
            throw std::runtime_error("fewer lines present than expected for an array format (" + std::to_string(my_nlines) + ")");
        }
        return true;
    }

    template<class Store_>
    bool scan_vector_array(Store_ store) {
        if (!my_passed_banner || !my_passed_size) {
            throw std::runtime_error("banner or size lines have not yet been parsed");
        }

        size_t current_data_line = 0;
        bool valid = my_input->valid();
        while (valid) {
            // Handling stray comments, empty lines, and leading spaces.
            if (!skip_lines()) {
                break;
            }
            if (!chomp()) {
                break;
            }

            if (current_data_line >= my_nlines) {
                throw std::runtime_error("more lines present than expected for an array format (" + std::to_string(my_nlines) + ")");
            }

            // 'store' should leave 'my_input' in the position immediately
            // after the successfully parsed value; this may or may not be EOL.
            auto res = store(current_data_line, 1);
            if (res.quit_early) {
                return true;
            }
            ++current_data_line;
            if (!res.remaining) {
                break;
            }

            // Cleaning up blanks in the rest of the line.
            if (!res.on_newline) {
                if (!chomp()) {
                    break;
                }
                if (my_input->get() != '\n') {
                    throw std::runtime_error("more fields than expected for a coordinate matrix on line " + std::to_string(my_current_line + 1));
                }
            }
            valid = my_input->advance(); // move past the newline.
        }

        if (current_data_line != my_nlines) {
            throw std::runtime_error("fewer lines present than expected for an array format (" + std::to_string(my_nlines) + ")");
        }
        return true;
    }

private:
    template<typename Type_>
    Type_ convert_to_real(const std::string& temporary) const {
        Type_ output;
        size_t n = 0;

        try {
            if constexpr(std::is_same<Type_, float>::value) {
                output = std::stof(temporary, &n);
            } else if constexpr(std::is_same<Type_, long double>::value) {
                output = std::stold(temporary, &n);
            } else {
                output = std::stod(temporary, &n);
            }
        } catch (std::invalid_argument& e) {
            throw std::runtime_error("failed to convert value to a real number on line " + std::to_string(my_current_line + 1));
        }

        if (n != temporary.size()) {
            throw std::runtime_error("failed to convert value to a real number on line " + std::to_string(my_current_line + 1));
        }

        return output;
    }

public:
    /**
     * Scan the file for integer lines, assuming that the field in the banner is `Field::INTEGER`.
     * 
     * @tparam Type_ Type to represent the integer.
     * @tparam Store_ Function to process each line.
     *
     * @param store Function with the signature `void(size_t row, size_t column, Type_ value)`,
     * which is passed the corresponding values at each line.
     * Both `row` and `column` will be 1-based indices; for `Object::VECTOR`, `column` will be set to 1.
     * Alternatively, this may return `bool`, where a `false` indicates that the scanning should terminate early and a `true` indicates that the scanning should continue.
     *
     * @return Whether the scanning terminated early, based on `store` returning `false`. 
     */
    template<typename Type_ = int, class Store_>
    bool scan_integer(Store_ store) {
        auto wrapper = [&](size_t r, size_t c) -> StoreInfo {
            bool negative = (my_input->get() == '-');
            if (!(my_input->advance())) {
                throw std::runtime_error("premature termination of an integer on line " + std::to_string(my_current_line + 1));
            }

            Type_ val = 0;
            bool found = false;
            auto finish = [&](bool valid) -> StoreInfo {
                if (!found) {
                    throw std::runtime_error("empty integer field on line " + std::to_string(my_current_line + 1));
                }
                StoreInfo output;
                output.quit_early = store(r, c, (negative ? -val : val));
                output.remaining = valid;
                return output;
            };

            while (1) {
                char x = my_input->get();
                switch (x) {
                    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                        val *= 10;
                        val += x - '0';
                        found = true;
                        break;
                    case ' ': case '\t':
                        if (!chomp()) {
                            return finish(false);
                        }
                        if (my_input->get() != '\n') {
                            throw std::runtime_error("more fields than expected for a coordinate matrix on line " + std::to_string(my_current_line + 1));
                        }
                        return finish(my_input->advance()); // move past the newline.
                    case '\n':
                        return finish(my_input->advance()); // move past the newline.
                    default:
                        throw std::runtime_error("expected an integer value on line " + std::to_string(my_current_line + 1));
                }

                if (!(my_input->advance())) {
                    break;
                }
            }

            return finish(false);
        };

        if (my_details.format == Format::COORDINATE) {
            if (my_details.object == Object::MATRIX) {
                return scan_matrix_coordinate_non_pattern(std::move(wrapper));
            } else {
                return scan_vector_coordinate_non_pattern(std::move(wrapper));
            }
        } else {
            if (my_details.object == Object::MATRIX) {
                return scan_matrix_array(std::move(wrapper));
            } else {
                return scan_vector_array(std::move(wrapper));
            }
        }
    }

    /**
     * Scan the file for real lines, assuming that the field in the banner is `Field::REAL`.
     * 
     * @tparam Type_ Type to represent the real value.
     * @tparam Store_ Function to process each line.
     *
     * @param store Function with the signature `void(size_t row, size_t column, Type_ value)`,
     * which is passed the corresponding values at each line.
     * Both `row` and `column` will be 1-based indices; for `Object::VECTOR`, `column` will be set to 1.
     * Alternatively, this may return `bool`, where a `false` indicates that the scanning should terminate early and a `true` indicates that the scanning should continue.
     *
     * @return Whether the scanning terminated early, based on `store` returning `false`. 
     */
    template<typename Type_ = double, class Store_>
    bool scan_real(Store_&& store) {
        std::string temporary;
        auto wrapper = [&](size_t r, size_t c, Input_& in) -> StoreInfo {
            StoreInfo output;
            output.remaining = true;

            temporary.clear();
            while (1) {
                char x = my_input->get();
                if (x == ' ' || x == '\t') {
                    if (!chomp()) {
                        output.remaining = false;
                        break;
                    }
                    if (my_input->get() != '\n') {
                        throw std::runtime_error("more fields than expected for a coordinate matrix on line " + std::to_string(my_current_line + 1));
                    }
                    output.remaining = my_input->advance(); // move past the newline.
                    break;
                } else if (x == '\n') {
                    output.remaining = my_input->advance(); // move past the newline.
                    break;
                }
                temporary += x;
                if (!(my_input->advance())) {
                    output.remaining = false;
                    break;
                }
            }

            if (temporary.empty()) {
                throw std::runtime_error("empty number field on line " + std::to_string(my_current_data_line + 1));
            }
            output.quit_early = store(r, c, convert_to_real<Type_>(temporary));
            return output;
        };

        if (my_details.format == Format::COORDINATE) {
            if (my_details.object == Object::MATRIX) {
                return scan_matrix_coordinate_non_pattern(std::move(wrapper));
            } else {
                return scan_vector_coordinate_non_pattern(std::move(wrapper));
            }
        } else {
            if (my_details.object == Object::MATRIX) {
                return scan_matrix_array(std::move(wrapper));
            } else {
                return scan_vector_array(std::move(wrapper));
            }
        }
    }

    /**
     * Scan the file for double-precision lines, assuming that the field in the banner is `Field::DOUBLE`.
     * This is just an alias for `scan_real()`.
     * 
     * @tparam Type_ Type to represent the double-precision value.
     * @tparam Store_ Function to process each line.
     *
     * @param store Function with the signature `void(size_t row, size_t column, Type_ value)`,
     * which is passed the corresponding values at each line.
     * Both `row` and `column` will be 1-based indices; for `Object::VECTOR`, `column` will be set to 1.
     * Alternatively, this may return `bool`, where a `false` indicates that the scanning should terminate early and a `true` indicates that the scanning should continue.
     *
     * @return Whether the scanning terminated early, based on `store` returning `false`. 
     */
    template<typename Type_ = double, class Store_>
    bool scan_double(Store_ store) {
        return scan_real<Type_, Store_>(std::move(store));
    }

    /**
     * Scan the file for complex lines, assuming that the field in the banner is `Field::COMPLEX`.
     * 
     * @tparam Type_ Type to represent the real and imaginary parts of the complex value.
     * @tparam Store_ Function to process each line.
     *
     * @param store Function with the signature `void(size_t row, size_t column, std::complex<Type_> value)`,
     * which is passed the corresponding values at each line.
     * Both `row` and `column` will be 1-based indices; for `Object::VECTOR`, `column` will be set to 1.
     * Alternatively, this may return `bool`, where a `false` indicates that the scanning should terminate early and a `true` indicates that the scanning should continue.
     *
     * @return Whether the scanning terminated early, based on `store` returning `false`. 
     */
    template<typename Type_ = double, class Store_>
    bool scan_complex(Store_ store) {
        std::string temporary;
        auto wrapper = [&](size_t r, size_t c, Input_& in) -> StoreInfo {
            StoreInfo output;
            output.remaining = true;
            std::complex<Type_> holding;

            // Pulling out the real part first.
            temporary.clear();
            while (1) {
                char x = my_input->get();
                if (x == ' ' || x == '\t') {
                    if (!chomp()) {
                        throw std::runtime_error("missing the imaginary part on line " + std::to_string(my_current_line + 1));
                    }
                    if (my_input->get() == '\n') {
                        throw std::runtime_error("missing the imaginary part on line " + std::to_string(my_current_line + 1));
                    }
                    break;
                } else if (x == '\n') {
                    throw std::runtime_error("missing the imaginary part on line " + std::to_string(my_current_data_line + 1));
                }
                temporary += x;
                if (!(my_input->advance())) {
                    throw std::runtime_error("expected a " + std::to_string(my_current_data_line + 1));
                }
            }
            if (temporary.empty()) {
                throw std::runtime_error("empty number field on line " + std::to_string(my_current_data_line + 1));
            }
            holding.real(convert_to_real<Type_>(temporary));

            // Now pulling out the imaginary part.
            temporary.clear();
            while (1) {
                char x = my_input->get();
                if (x == ' ' || x == '\t') {
                    if (!chomp()) {
                        output.remaining = false;
                        break;
                    }
                    if (my_input->get() != '\n') {
                        throw std::runtime_error("more fields than expected for a coordinate matrix on line " + std::to_string(my_current_line + 1));
                    }
                    output.remaining = my_input->advance(); // skipping past the newline.
                    break;
                } else if (x == '\n') {
                    output.remaining = my_input->advance(); // skipping past the newline.
                    break;
                }
                temporary += x;
                if (!(my_input->advance())) {
                    output.remaining = false;
                    break;
                }
            }
            if (temporary.empty()) {
                throw std::runtime_error("empty number field on line " + std::to_string(my_current_data_line + 1));
            }
            holding.imag(convert_to_real<Type_>(temporary));

            output.quit_early = store(r, c, holding);
            return output;
        };

        if (my_details.format == Format::COORDINATE) {
            if (my_details.object == Object::MATRIX) {
                return scan_matrix_coordinate_non_pattern(std::move(wrapper));
            } else {
                return scan_vector_coordinate_non_pattern(std::move(wrapper));
            }
        } else {
            if (my_details.object == Object::MATRIX) {
                return scan_matrix_array(std::move(wrapper));
            } else {
                return scan_vector_array(std::move(wrapper));
            }
        }
    }

    /**
     * Scan the file for pattern lines, assuming that the field in the banner is `Field::PATTERN`.
     * This function only works when the format field is set to `Format::COORDINATE`.
     * 
     * @tparam Type_ Type to represent the presence of a non-zero entry.
     * @tparam Store_ Function to process each line.
     *
     * @param store Function with the signature `void(size_t row, size_t column, Type_ value)`,
     * which is passed the corresponding values at each line.
     * Both `row` and `column` will be 1-based indices; for `Object::VECTOR`, `column` will be set to 1.
     * `value` will always be `true` and can be ignored; it is only required here for consistency with the other methods.
     * Alternatively, this may return `bool`, where a `false` indicates that the scanning should terminate early and a `true` indicates that the scanning should continue.
     *
     * @return Whether the scanning terminated early, based on `store` returning `false`. 
     */
    template<typename Type_ = bool, class Store_>
    bool scan_pattern(Store_ store) {
        if (my_details.format != Format::COORDINATE) {
            throw std::runtime_error("'array' format for 'pattern' field is not supported");
        }
        if (my_details.object == Object::MATRIX) {
            return scan_matrix_coordinate_pattern(std::move(store));
        } else {
            return scan_vector_coordinate_pattern(std::move(store));
        }
    }
};

}

#endif
