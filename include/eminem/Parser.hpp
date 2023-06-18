#ifndef EMINEM_PARSER_HPP
#define EMINEM_PARSER_HPP

#include <vector>
#include <string>
#include <cstring>

#include "byteme/PerByte.hpp"
#include "utils.hpp"

/**
 * @file simple.hpp
 *
 * @brief Parse a matrix from the Matrix Market coordinate format.
 */

namespace eminem {

template<bool parallel_ = false>
class Parser {
public:
    Parser(byteme::Reader& r) : input(r) {}

private:
    typename std::conditional<parallel_, byteme::PerByteParallel<>, byteme::PerByte<> >::type input;
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
                ncols = 0;
            } else if (details.format == Format::ARRAY) {
                if (onto != 0) {
                    throw std::runtime_error("expected one size field for array vectors on line " + std::to_string(current_line + 1));
                }
                nlines = nrows;
            }
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
    size_t get_nrows() const {
        if (!passed_size) {
            throw std::runtime_error("size line has not yet been scanned");
        }
        return nrows;
    }

    size_t get_ncols() const {
        if (!passed_size) {
            throw std::runtime_error("size line has not yet been scanned");
        }
        return ncols;
    }

    size_t get_nlines() const {
        if (!passed_size) {
            throw std::runtime_error("size line has not yet been scanned");
        }
        return nlines;
    }

public:
    void scan_preamble() {
        scan_banner();
        scan_size();
        return;
    }

private:
    void check_coordinates(size_t currow, size_t curcol, size_t current_data_line, int onto, bool non_empty) const {
        if (!non_empty) {
            throw std::runtime_error("empty field detected on line " + std::to_string(current_line + 1));
        }

        switch (details.field) {
            case Field::REAL: case Field::DOUBLE: case Field::INTEGER:
                if (onto != 2) {
                    throw std::runtime_error("expected 3 fields on line " + std::to_string(current_line + 1));
                }
                break;
            case Field::PATTERN:
                if (onto != 2) {
                    throw std::runtime_error("expected 2 fields on line " + std::to_string(current_line + 1));
                }
                break;
            case Field::COMPLEX:
                if (onto != 4) {
                    throw std::runtime_error("expected 4 fields on line " + std::to_string(current_line + 1));
                }
                break;
        }

        if (!currow) {
            throw std::runtime_error("row index must be positive on line " + std::to_string(current_line + 1));
        }
        if (currow > nrows) {
            throw std::runtime_error("row index out of range on line " + std::to_string(current_line + 1));
        }

        if (!curcol) {
            throw std::runtime_error("column index must be positive on line " + std::to_string(current_line + 1));
        }
        if (curcol > ncols) {
            throw std::runtime_error("column index out of range on line " + std::to_string(current_line + 1));
        }

        if (current_data_line >= nlines) {
            throw std::runtime_error("more lines present than specified in the header (" + std::to_string(nlines) + ")");
        }
    }

    void reset_coordinates(size_t& currow, size_t& curcol, size_t& current_data_line, char& onto, bool& non_empty) const {
        currow = 0;
        curcol = 0;
        ++current_data_line;
        onto = 0;
        non_empty = false;
    }

private:
    template<class Store_, class Compose_, class Bump_, class Finish_>
    void scan_coordinate(Store_ store, Compose_ compose, Bump_ bump, Finish_ finish) {
        if (!passed_banner || !passed_size) {
            throw std::runtime_error("banner or size lines have not yet been parsed");
        }

        size_t current_data_line = 0;
        size_t currow = 0, curcol = 0;
        char onto = 0;
        bool non_empty = false;
        bool valid = input.valid();

        // 'position', 'remaining', etc. should be filled from previous scan_banner() call.
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
                    if (onto >= 2) {
                        bump(current_line);
                    }
                    ++onto;
                    non_empty = false;

                } else if (current == '\n') {
                    check_coordinates(currow, curcol, current_data_line, onto, non_empty);
                    store(currow, curcol, finish(current_line));
                    reset_coordinates(currow, curcol, current_data_line, onto, non_empty);
                    ++current_line;
                    valid = input.advance();
                    break;

                } else {
                    switch (onto) {
                        case 0:
                            if (current < '0' || current > '9') {
                                throw std::runtime_error("row index should be a non-negative integer on line " + std::to_string(current_line));
                            }
                            currow *= 10;
                            currow += current - '0';
                            break;

                        case 1:
                            if (current < '0' || current > '9') {
                                throw std::runtime_error("column index should be a non-negative integer on line " + std::to_string(current_line));
                            }
                            curcol *= 10;
                            curcol += current - '0';
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
            check_coordinates(currow, curcol, current_data_line, onto, non_empty);
            store(currow, curcol, finish(current_line));
            reset_coordinates(currow, curcol, current_data_line, onto, non_empty);
        }

        if (current_data_line != nlines) {
            throw std::runtime_error("fewer lines present than specified in the header (" + std::to_string(nlines) + ")");
        }
        return;
    }

public:
    template<typename Type_ = int, class Store_>
    void scan_integer(Store_&& store) {
        bool init = true;
        bool negative = false;
        Type_ curval = 0;

        scan_coordinate(
            std::forward<Store_>(store), 
            /* compose = */ [&](char x, size_t line) -> void {
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
            },
            /* bump = */ [](size_t) -> void {}, 
            /* finish = */ [&](size_t) -> Type_ {
                if (negative) {
                    curval *= -1;
                }
                init = true;
                negative = false;

                auto copy = curval;
                curval = 0;
                return copy;
            }
        );
    }

    template<typename Type_ = double, class Store_>
    void scan_real(Store_&& store) {
        std::string temporary;

        scan_coordinate(
            std::forward<Store_>(store), 
            /* compose = */ [&](char x, size_t line) -> void {
                if (std::isspace(x)) {
                    throw std::runtime_error("detected whitespace in value on line " + std::to_string(line + 1));
                }
                temporary += x;
            },
            /* bump = */ [](size_t) -> void {}, 
            /* finish = */ [&](size_t line) -> double {
                double output;
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
                    throw std::runtime_error("failed to convert value to a double on line " + std::to_string(line + 1));
                }

                if (n != temporary.size()) {
                    throw std::runtime_error("failed to convert value to a double on line " + std::to_string(line + 1));
                }
                temporary.clear();
                return output;
            }
        );
    }
};

}

#endif
