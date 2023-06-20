#ifndef EMINEM_PARSER_HPP
#define EMINEM_PARSER_HPP

#include <vector>
#include <string>
#include <cstring>
#include <complex>

#include "byteme/PerByte.hpp"
#include "utils.hpp"

/**
 * @file Parser.hpp
 *
 * @brief Parse a matrix from a Matrix Market file.
 */

namespace eminem {

/**
 * @brief Parse a matrix from a Matrix Market file.
 *
 * @tparam parallel_ Whether to parallelize the byte reading and parsing.
 * If `true`, a separate thread is used to read the bytes from the input source.
 * @tparam Pointer_ A (possibly smart) pointer to a `byteme::Reader` instance.
 */
template<bool parallel_ = false, class Pointer_ = byteme::Reader*>
class Parser {
public:
    /**
     * @param r Pointer to an unused `Reader` instance from the [**byteme**](https://github.com/LTLA/byteme) library.
     */
    Parser(Pointer_ r) : input(std::move(r)) {}

private:
    typename std::conditional<parallel_, byteme::PerByteParallel<char, Pointer_>, byteme::PerByte<char, Pointer_> >::type input;
    size_t current_line = 0;

private:
    bool passed_banner = false;
    MatrixDetails details;

    void parse_banner(const std::string& contents) {
        size_t pos = 13; // skip the "MatrixMarket " string.
        size_t len = contents.size();

        auto obj_str = contents.c_str() + pos;
        if (std::strncmp(obj_str, "matrix ", 7) == 0) {
            details.object = Object::MATRIX;
            pos += 7;
        } else if (std::strncmp(obj_str, "vector ", 7) == 0) {
            details.object = Object::VECTOR;
            pos += 7;
        } else {
            throw std::runtime_error("first banner field should be one of 'matrix' or 'vector'");
        }

        auto format_str = contents.c_str() + pos;
        if (std::strncmp(format_str, "coordinate ", 11) == 0) {
            details.format = Format::COORDINATE;
            pos += 11;
        } else if (std::strncmp(format_str, "array ", 6) == 0) {
            details.format = Format::ARRAY;
            pos += 6;
        } else {
            throw std::runtime_error("second banner field should be one of 'coordinate' or 'array'");
        }

        auto field_str = contents.c_str() + pos;
        if (std::strncmp(field_str, "integer ", 8) == 0) {
            details.field = Field::INTEGER;
            pos += 8;
        } else if (std::strncmp(field_str, "double ", 7) == 0) {
            details.field = Field::DOUBLE;
            pos += 7;
        } else if (std::strncmp(field_str, "complex ", 8) == 0) {
            details.field = Field::COMPLEX;
            pos += 8;
        } else if (std::strncmp(field_str, "pattern ", 8) == 0) {
            details.field = Field::PATTERN;
            pos += 8;
        } else if (std::strncmp(field_str, "real ", 5) == 0) {
            details.field = Field::REAL;
            pos += 5;
        } else {
            throw std::runtime_error("third banner field should be one of 'real', 'integer', 'double', 'complex' or 'pattern'");
        }

        auto sym_str = contents.c_str() + pos;
        if (std::strcmp(sym_str, "general") == 0) {
            details.symmetry = Symmetry::GENERAL;
        } else if (std::strcmp(sym_str, "symmetric") == 0) {
            details.symmetry = Symmetry::SYMMETRIC;
        } else if (std::strcmp(sym_str, "skew-symmetric") == 0) {
            details.symmetry = Symmetry::SKEW_SYMMETRIC;
        } else if (std::strcmp(sym_str, "hermitian") == 0) {
            details.symmetry = Symmetry::HERMITIAN;
        } else {
            throw std::runtime_error("fourth banner field should be one of 'general', 'symmetric', 'skew-symemtric' or 'hermitian'");
        }
    }

    void scan_banner() {
        std::string contents;
        bool valid = input.valid();
        if (passed_banner) {
            throw std::runtime_error("banner has already been scanned");
        }

        while (valid) {
            if (input.get() != '%') {
                throw std::runtime_error("failed to find banner line before non-commented line " + std::to_string(current_line + 1));
            }

            // Exhaust all leading comment characters.
            do {
                valid = input.advance();
            } while (valid && input.get() == '%');

            // Inserting up to the next line.
            while (valid && input.get() != '\n') {
                contents += input.get();
                valid = input.advance();
            }

            if (!valid) {
                break;
            }
            ++current_line;
            valid = input.advance();

            if (contents.rfind("MatrixMarket ", 0) == 0) {
                parse_banner(contents);
                passed_banner = true;
                return;
            } 
            contents.clear();
        } 

        throw std::runtime_error("failed to find banner line before end of file");
    }

public:
    /**
     * Retrieve the Matrix Market banner.
     * This should be called after `scan_preamble()`.
     *
     * @return Details about the matrix in this file.
     */
    const MatrixDetails& get_banner() const {
        if (!passed_banner) {
            throw std::runtime_error("banner has not yet been scanned");
        }
        return details;
    }

private:
    bool passed_size = false;
    size_t nrows = 0, ncols = 0, nlines = 0;

    void check_size(int onto, bool non_empty) {
        if (!non_empty) {
            throw std::runtime_error("detected an empty size field on line " + std::to_string(current_line + 1));
        }

        if (details.object == Object::MATRIX) {
            if (details.format == Format::COORDINATE) {
                if (onto != 2) {
                    throw std::runtime_error("expected three size fields for coordinate matrices on line " + std::to_string(current_line + 1));
                }
            } else if (details.format == Format::ARRAY) {
                if (onto != 1) {
                    throw std::runtime_error("expected two size fields for array matrices on line " + std::to_string(current_line + 1));
                }
                nlines = nrows * ncols;
            }
        } else {
            if (details.format == Format::COORDINATE) {
                if (onto != 1) {
                    throw std::runtime_error("expected two size fields for coordinate vectors on line " + std::to_string(current_line + 1));
                }
                nlines = ncols;
            } else if (details.format == Format::ARRAY) {
                if (onto != 0) {
                    throw std::runtime_error("expected one size field for array vectors on line " + std::to_string(current_line + 1));
                }
                nlines = nrows;
            }
            ncols = 1;
        }

        passed_size = true;
    }

    void scan_size() {
        char onto = 0;
        bool non_empty = false;
        bool valid = input.valid();

        while (valid) {
            if (input.get() == '%') {
                // Handling stray comments... try to get to the next line as quickly as possible.
                do {
                    valid = input.advance();
                } while (valid && input.get() != '\n');

                if (!valid) {
                    break;
                } else {
                    ++current_line;
                    valid = input.advance();
                    continue;
                }
            }

            // Chomping digits.
            do {
                char current = input.get();
                switch(current) {
                    case ' ':
                        if (!non_empty) {
                            throw std::runtime_error("detected an empty size field on line " + std::to_string(current_line + 1));
                        }
                        ++onto;
                        break;

                    case '\n':
                        ++current_line;
                        valid = input.advance();
                        check_size(onto, non_empty);
                        return;

                    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                        {
                            non_empty = true;
                            auto delta = current - '0';
                            switch(onto) {
                                case 0:
                                    nrows *= 10;
                                    nrows += delta;
                                    break;
                                case 1:
                                    ncols *= 10;
                                    ncols += delta;
                                    break;
                                case 2:
                                    nlines *= 10;
                                    nlines += delta;
                                    break;
                            }
                        }
                        break;

                    default:
                        throw std::runtime_error("only non-negative integers should be present on line " + std::to_string(current_line + 1));
                }

                valid = input.advance();
            } while (valid);
        }

        check_size(onto, non_empty);
    }

public:
    /**
     * Extract the number of rows in the matrix.
     * This should be called after `scan_preamble()`.
     * If the object type is `Object::VECTOR`, the number of rows is equal to the length of the vector.
     *
     * @return Number of rows.
     */
    size_t get_nrows() const {
        if (!passed_size) {
            throw std::runtime_error("size line has not yet been scanned");
        }
        return nrows;
    }

    /**
     * Extract the number of columns in the matrix.
     * This should be called after `scan_preamble()`.
     * If the object type is `Object::VECTOR`, the number of columns is set to 1.
     *
     * @return Number of columns.
     */
    size_t get_ncols() const {
        if (!passed_size) {
            throw std::runtime_error("size line has not yet been scanned");
        }
        return ncols;
    }

    /**
     * Extract the number of non-zero lines in the coordinate format.
     * This should be called after `scan_preamble()`.
     * If the object type is `Object::ARRAY`, the number of lines is set to the produce of the number of rows and columns.
     *
     * @return Number of non-zero lines. 
     */
    size_t get_nlines() const {
        if (!passed_size) {
            throw std::runtime_error("size line has not yet been scanned");
        }
        return nlines;
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
    void check_coordinate_common(size_t currow, size_t current_data_line, bool non_empty) const {
        if (!non_empty) {
            throw std::runtime_error("empty field detected on line " + std::to_string(current_line + 1));
        }

        if (current_data_line >= nlines) {
            throw std::runtime_error("more lines present than specified in the header (" + std::to_string(nlines) + ")");
        }

        if (!currow) {
            throw std::runtime_error("row index must be positive on line " + std::to_string(current_line + 1));
        }
        if (currow > nrows) {
            throw std::runtime_error("row index out of range on line " + std::to_string(current_line + 1));
        }
    }

    template<Field field_>
    void check_coordinate_matrix(size_t currow, size_t curcol, size_t current_data_line, int onto, bool non_empty) const {
        check_coordinate_common(currow, current_data_line, non_empty);

        if constexpr(field_ == Field::REAL || field_ == Field::DOUBLE || field_ == Field::INTEGER) {
            if (onto != 2) {
                throw std::runtime_error("expected 3 fields on line " + std::to_string(current_line + 1));
            }
        } else if constexpr(field_ == Field::PATTERN) {
            if (onto != 1) {
                throw std::runtime_error("expected 2 fields on line " + std::to_string(current_line + 1));
            }
        } else if constexpr(field_ == Field::COMPLEX) {
            if (onto != 3) {
                throw std::runtime_error("expected 4 fields on line " + std::to_string(current_line + 1));
            }
        }

        if (!curcol) {
            throw std::runtime_error("column index must be positive on line " + std::to_string(current_line + 1));
        }
        if (curcol > ncols) {
            throw std::runtime_error("column index out of range on line " + std::to_string(current_line + 1));
        }
    }

    template<Field field_>
    void check_coordinate_vector(size_t currow, size_t current_data_line, int onto, bool non_empty) const {
        check_coordinate_common(currow, current_data_line, non_empty);

        if constexpr(field_ == Field::REAL || field_ == Field::DOUBLE || field_ == Field::INTEGER) {
            if (onto != 1) {
                throw std::runtime_error("expected 2 fields on line " + std::to_string(current_line + 1));
            }
        } else if constexpr(field_ == Field::PATTERN) {
            if (onto != 0) {
                throw std::runtime_error("expected 1 field on line " + std::to_string(current_line + 1));
            }
        } else if constexpr(field_ == Field::COMPLEX) {
            if (onto != 2) {
                throw std::runtime_error("expected 3 fields on line " + std::to_string(current_line + 1));
            }
        }
    }

    template<Object object_, Field field_, class Store_, class Compose_, class Bump_, class Finish_>
    void scan_coordinate(Store_ store, Compose_ compose, Bump_ bump, Finish_ finish) {
        if (!passed_banner || !passed_size) {
            throw std::runtime_error("banner or size lines have not yet been parsed");
        }

        size_t current_data_line = 0;
        size_t currow = 0, curcol = 0;
        char onto = 0;
        bool non_empty = false;
        bool valid = input.valid();

        while (valid) {
            if (input.get() == '%') {
                // Try to get quickly to the next line.
                do {
                    valid = input.advance();
                } while (valid && input.get() != '\n');

                if (!valid) {
                    break;
                } else {
                    ++current_line;
                    valid = input.advance();
                    continue;
                }
            }

            do {
                char current = input.get();
                if (current == ' ') {
                    if (!non_empty) {
                        throw std::runtime_error("detected empty field on line " + std::to_string(current_line + 1));
                    }
                    if constexpr(object_ == Object::MATRIX) {
                        if (onto >= 2) {
                            bump(current_line);
                        }
                    } else {
                        if (onto >= 1) {
                            bump(current_line);
                        }
                    }
                    ++onto;
                    non_empty = false;

                } else if (current == '\n') {
                    if constexpr(object_ == Object::MATRIX) {
                        check_coordinate_matrix<field_>(currow, curcol, current_data_line, onto, non_empty);
                        store(currow, curcol, finish(current_line));
                        curcol = 0;
                    } else {
                        check_coordinate_vector<field_>(currow, current_data_line, onto, non_empty);
                        store(currow, 1, finish(current_line));
                    }

                    currow = 0;
                    ++current_data_line;
                    onto = 0;
                    non_empty = false;
                    ++current_line;
                    valid = input.advance();
                    break;

                } else {
                    switch (onto) {
                        case 0:
                            if (current < '0' || current > '9') {
                                throw std::runtime_error("row index should be a non-negative integer on line " + std::to_string(current_line + 1));
                            }
                            currow *= 10;
                            currow += current - '0';
                            break;

                        case 1:
                            if constexpr(object_ == Object::MATRIX) {
                                if (current < '0' || current > '9') {
                                    throw std::runtime_error("column index should be a non-negative integer on line " + std::to_string(current_line + 1));
                                }
                                curcol *= 10;
                                curcol += current - '0';
                            } else {
                                compose(current, current_line);
                            }
                            break;

                        default:
                            compose(current, current_line);
                            break;
                    }
                    non_empty = true;
                }

                valid = input.advance();
            } while (valid);
        }

        // If onto = 0 and non_empty = false, we ended on a newline, so 
        // there's no extra entry to add. Otherwise, we try to add the 
        // last line that was _not_ terminated by a newline.
        if (onto != 0 || non_empty) { 
            if constexpr(object_ == Object::MATRIX) {
                check_coordinate_matrix<field_>(currow, curcol, current_data_line, onto, non_empty);
                store(currow, curcol, finish(current_line));
            } else {
                check_coordinate_vector<field_>(currow, current_data_line, onto, non_empty);
                store(currow, 1, finish(current_line));
            }
            ++current_data_line;
        }

        if (current_data_line != nlines) {
            throw std::runtime_error("fewer lines present than specified in the header (" + std::to_string(nlines) + ")");
        }
        return;
    }

private:
    template<Field field_>
    void check_array(size_t current_data_line, int onto, bool non_empty) const {
        if (!non_empty) {
            throw std::runtime_error("empty field detected on line " + std::to_string(current_line + 1));
        }

        if constexpr(field_ == Field::REAL || field_ == Field::DOUBLE || field_ == Field::INTEGER) {
            if (onto != 0) {
                throw std::runtime_error("expected 1 field on line " + std::to_string(current_line + 1));
            }
        } else if constexpr(field_ == Field::COMPLEX) {
            if (onto != 1) {
                throw std::runtime_error("expected 2 fields on line " + std::to_string(current_line + 1));
            }
        }

        if (current_data_line >= nlines) {
            throw std::runtime_error("more lines present than expected for an array format (" + std::to_string(nlines) + ")");
        }
    }

    template<Field field_, class Store_, class Compose_, class Bump_, class Finish_>
    void scan_array(Store_ store, Compose_ compose, Bump_ bump, Finish_ finish) {
        if (!passed_banner || !passed_size) {
            throw std::runtime_error("banner or size lines have not yet been parsed");
        }

        size_t current_data_line = 0;
        size_t currow = 1, curcol = 1;
        char onto = 0;
        bool non_empty = false;
        bool valid = input.valid();

        while (valid) {
            if (input.get() == '%') {
                // Try to get quickly to the next line.
                do {
                    valid = input.advance();
                } while (valid && input.get() != '\n');

                if (!valid) {
                    break;
                } else {
                    ++current_line;
                    valid = input.advance();
                    continue;
                }
            }

            do {
                char current = input.get();
                if (current == ' ') {
                    if (!non_empty) {
                        throw std::runtime_error("detected empty field on line " + std::to_string(current_line + 1));
                    }
                    bump(current_line);
                    ++onto;
                    non_empty = false;

                } else if (current == '\n') {
                    check_array<field_>(current_data_line, onto, non_empty); 
                    store(currow, curcol, finish(current_line));

                    ++currow;
                    if (currow > nrows) {
                        ++curcol;
                        currow = 1;
                    }
                    ++current_data_line;
                    ++current_line;
                    onto = 0;
                    non_empty = false;

                    valid = input.advance();
                    break;

                } else {
                    compose(current, current_line);
                    non_empty = true;
                }

                valid = input.advance();
            } while (valid);
        }

        // If onto = 0 and non_empty = false, we ended on a newline, so 
        // there's no extra entry to add. Otherwise, we try to add the 
        // last line that was _not_ terminated by a newline.
        if (onto != 0 || non_empty) { 
            check_array<field_>(current_data_line, onto, non_empty); 
            store(currow, curcol, finish(current_line));
            ++current_data_line;
        }

        if (current_data_line != nlines) {
            throw std::runtime_error("fewer lines present than expected for an array format (" + std::to_string(nlines) + ")");
        }
        return;
    }

private:
    template<typename Type_>
    static Type_ convert_to_real(const std::string& temporary, size_t line) {
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
            throw std::runtime_error("failed to convert value to a real number on line " + std::to_string(line + 1));
        }

        if (n != temporary.size()) {
            throw std::runtime_error("failed to convert value to a real number on line " + std::to_string(line + 1));
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
     * For `Object::VECTOR`, `column` is fixed to 1.
     */
    template<typename Type_ = int, class Store_>
    void scan_integer(Store_&& store) {
        bool init = true;
        bool negative = false;
        Type_ curval = 0;

        auto compose = [&](char x, size_t line) -> void {
            if (init) {
                if (x == '-') {
                    negative = true;
                    return;
                }
                init = false;
            }
            if (x < '0' || x > '9') {
                throw std::runtime_error("expected an integer value on line " + std::to_string(line + 1));
            }
            curval *= 10;
            curval += (x - '0'); 
        };

        auto bump = [](size_t) -> void {};

        auto finish = [&](size_t) -> Type_ {
            if (negative) {
                curval *= -1;
            }
            init = true;
            negative = false;

            auto copy = curval;
            curval = 0;
            return copy;
        };

        if (details.format == Format::COORDINATE) {
            if (details.object == Object::MATRIX) {
                scan_coordinate<Object::MATRIX, Field::INTEGER>(std::forward<Store_>(store), std::move(compose), std::move(bump), std::move(finish));
            } else {
                scan_coordinate<Object::VECTOR, Field::INTEGER>(std::forward<Store_>(store), std::move(compose), std::move(bump), std::move(finish));
            }
        } else {
            scan_array<Field::INTEGER>(std::forward<Store_>(store), std::move(compose), std::move(bump), std::move(finish));
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
     * For `Object::VECTOR`, `column` is fixed to 1.
     */
    template<typename Type_ = double, class Store_>
    void scan_real(Store_&& store) {
        std::string temporary;

        auto compose = [&](char x, size_t line) -> void {
            if (std::isspace(x)) {
                throw std::runtime_error("detected whitespace in value on line " + std::to_string(line + 1));
            }
            temporary += x;
        };

        auto bump = [](size_t) -> void {};

        auto finish = [&](size_t line) -> double {
            double output = convert_to_real<Type_>(temporary, line);
            temporary.clear();
            return output;
        };

        if (details.format == Format::COORDINATE) {
            if (details.object == Object::MATRIX) {
                scan_coordinate<Object::MATRIX, Field::REAL>(std::forward<Store_>(store), std::move(compose), std::move(bump), std::move(finish));
            } else {
                scan_coordinate<Object::VECTOR, Field::REAL>(std::forward<Store_>(store), std::move(compose), std::move(bump), std::move(finish));
            }
        } else {
            scan_array<Field::REAL>(std::forward<Store_>(store), std::move(compose), std::move(bump), std::move(finish));
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
     * For `Object::VECTOR`, `column` is fixed to 1.
     */
    template<typename Type_ = double, class Store_>
    void scan_double(Store_&& store) {
        scan_real<Type_, Store_>(std::forward<Store_>(store));
    }

    /**
     * Scan the file for complex lines, assuming that the field in the banner is `Field::COMPLEX`.
     * 
     * @tparam Type_ Type to represent the real and imaginary parts of the complex value.
     * @tparam Store_ Function to process each line.
     *
     * @param store Function with the signature `void(size_t row, size_t column, std::complex<Type_> value)`,
     * which is passed the corresponding values at each line.
     * For `Object::VECTOR`, `column` is fixed to 1.
     */
    template<typename Type_ = double, class Store_>
    void scan_complex(Store_&& store) {
        std::string temporary;
        std::complex<Type_> holding;

        auto compose = [&](char x, size_t line) -> void {
            if (std::isspace(x)) {
                throw std::runtime_error("detected whitespace in value on line " + std::to_string(line + 1));
            }
            temporary += x;
        };

        auto bump = [&](size_t line) -> void {
            holding.real(convert_to_real<Type_>(temporary, line));
            temporary.clear();
        };

        auto finish = [&](size_t line) -> std::complex<Type_> {
            holding.imag(convert_to_real<Type_>(temporary, line));
            temporary.clear();
            return holding;
        };

        if (details.format == Format::COORDINATE) {
            if (details.object == Object::MATRIX) {
                scan_coordinate<Object::MATRIX, Field::COMPLEX>(std::forward<Store_>(store), std::move(compose), std::move(bump), std::move(finish));
            } else {
                scan_coordinate<Object::VECTOR, Field::COMPLEX>(std::forward<Store_>(store), std::move(compose), std::move(bump), std::move(finish));
            }
        } else {
            scan_array<Field::COMPLEX>(std::forward<Store_>(store), std::move(compose), std::move(bump), std::move(finish));
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
     * `value` is always set to `true` and can be ignored; it is only listed here for consistency with the other methods.
     */
    template<typename Type_ = bool, class Store_>
    void scan_pattern(Store_&& store) {
        auto compose = [](char, size_t) -> void {};
        auto bump = [](size_t) -> void {};
        auto finish = [](size_t) -> Type_ { 
            return true; 
        };

        if (details.format == Format::COORDINATE) {
            if (details.object == Object::MATRIX) {
                scan_coordinate<Object::MATRIX, Field::PATTERN>(std::forward<Store_>(store), std::move(compose), std::move(bump), std::move(finish));
            } else {
                scan_coordinate<Object::VECTOR, Field::PATTERN>(std::forward<Store_>(store), std::move(compose), std::move(bump), std::move(finish));
            }
        } else {
            throw std::runtime_error("'array' format for 'pattern' field is not supported");
        }
    }
};

}

#endif
