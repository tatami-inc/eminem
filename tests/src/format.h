#ifndef EMINEM_TEST_FORMAT_H 
#define EMINEM_TEST_FORMAT_H

#include <vector>
#include <type_traits>
#include <random>
#include <complex>
#include <iomanip>

template<class Stream_, typename Value_>
void add_field(Stream_& stream) {
    if constexpr(std::is_same<Value_, int>::value) {
        stream << "integer";
    } else if constexpr(std::is_same<Value_, double>::value) {
        stream << "double";
        stream << std::setprecision(10);
    } else if constexpr(std::is_same<Value_, std::complex<double> >::value) {
        stream << "complex";
        stream << std::setprecision(10);
    } else {
        stream << "pattern";
    }
}

template<class Stream_, typename Rng_>
bool add_variable_space(Stream_& stream, Rng_& rng) {
    bool added = false;
    while (rng() % 100 < 50) {
        added = true;
        stream << " ";
    }
    while (rng() % 100 < 50) {
        added = true;
        stream << "\t";
    }
    return added;
}

template<class Stream_, typename Rng_>
void add_at_least_one_variable_space(Stream_& stream, Rng_& rng) {
    if (!add_variable_space(stream, rng)) {
        stream << " ";
    }
}

template<class Stream_, typename Rng_>
void add_at_least_one_newline(Stream_& stream, Rng_& rng) {
    stream << "\n";
    while (rng() % 100 < 20) {
        if (rng() % 100 < 50) {
            stream << "\n";
        } else {
            stream << "%hi i am a comment\n";
        }
    }
}

template<class Stream_, typename Value_>
void format_coordinate(Stream_& stream, size_t nr, size_t nc, const std::vector<int>& rows, const std::vector<int>& cols, const std::vector<Value_>& vals) {
    std::mt19937_64 rng(nr * nc);

    stream << "%%MatrixMarket";
    add_at_least_one_variable_space(stream, rng);
    stream << "matrix";
    add_at_least_one_variable_space(stream, rng);
    stream << "coordinate";
    add_at_least_one_variable_space(stream, rng);
    add_field<Stream_, Value_>(stream);
    add_at_least_one_variable_space(stream, rng);
    stream << " general";
    add_variable_space(stream, rng);
    add_at_least_one_newline(stream, rng);

    add_variable_space(stream, rng);
    stream << nr;
    add_at_least_one_variable_space(stream, rng);
    stream << nc;
    add_at_least_one_variable_space(stream, rng);
    stream << rows.size();
    add_variable_space(stream, rng);

    for (size_t i = 0; i < rows.size(); ++i) {
        add_at_least_one_newline(stream, rng);
        add_variable_space(stream, rng);
        stream << rows[i] + 1;
        add_at_least_one_variable_space(stream, rng);
        stream << cols[i] + 1;

        if constexpr(!std::is_same<Value_, char>::value) {
            add_at_least_one_variable_space(stream, rng);
            if constexpr(std::is_same<Value_, int>::value || std::is_same<Value_, double>::value) {
                stream << vals[i];
            } else if constexpr(std::is_same<Value_, std::complex<double> >::value) {
                stream << vals[i].real();
                add_at_least_one_variable_space(stream, rng);
                stream << vals[i].imag();
            }
        }

        add_variable_space(stream, rng);
    }

    if (rng() % 100 < 50) {
        stream << "\n"; // maybe add a terminating newline.
    }
}

template<class Stream_, typename Value_>
void format_coordinate(Stream_& stream, size_t n, const std::vector<int>& entries, const std::vector<Value_>& vals) {
    std::mt19937_64 rng(n);

    stream << "%%MatrixMarket";
    add_at_least_one_variable_space(stream, rng);
    stream << "vector";
    add_at_least_one_variable_space(stream, rng);
    stream << "coordinate";
    add_at_least_one_variable_space(stream, rng);
    add_field<Stream_, Value_>(stream);
    add_variable_space(stream, rng);
    add_at_least_one_newline(stream, rng);

    add_variable_space(stream, rng);
    stream << n;
    add_at_least_one_variable_space(stream, rng);
    stream << entries.size();
    add_variable_space(stream, rng);

    for (size_t i = 0; i < entries.size(); ++i) {
        add_at_least_one_newline(stream, rng);
        add_variable_space(stream, rng);
        stream << entries[i] + 1;

        if constexpr(!std::is_same<Value_, char>::value) {
            add_at_least_one_variable_space(stream, rng);
            if constexpr(std::is_same<Value_, int>::value || std::is_same<Value_, double>::value) {
                stream << vals[i];
            } else if constexpr(std::is_same<Value_, std::complex<double> >::value) {
                stream << vals[i].real();
                add_at_least_one_variable_space(stream, rng);
                stream << vals[i].imag();
            }
        }

        add_variable_space(stream, rng);
    }

    if (rng() % 100 < 50) {
        stream << "\n"; // maybe add a terminating newline.
    }
}

template<class Stream_, typename Value_>
void format_array(Stream_& stream, size_t nr, size_t nc, const std::vector<Value_>& vals) {
    std::mt19937_64 rng(nr * nc);

    stream << "%%MatrixMarket";
    add_at_least_one_variable_space(stream, rng);
    stream << "matrix";
    add_at_least_one_variable_space(stream, rng);
    stream << "array";
    add_at_least_one_variable_space(stream, rng);
    add_field<Stream_, Value_>(stream);
    add_at_least_one_variable_space(stream, rng);
    stream << " general";
    add_variable_space(stream, rng);
    add_at_least_one_newline(stream, rng);

    add_variable_space(stream, rng);
    stream << nr;
    add_at_least_one_variable_space(stream, rng);
    stream << nc;
    add_variable_space(stream, rng);

    for (size_t i = 0; i < vals.size(); ++i) {
        add_at_least_one_newline(stream, rng);
        add_variable_space(stream, rng);
        if constexpr(std::is_same<Value_, int>::value || std::is_same<Value_, double>::value) {
            stream << vals[i];
        } else if constexpr(std::is_same<Value_, std::complex<double> >::value) {
            stream << vals[i].real();
            add_at_least_one_variable_space(stream, rng);
            stream << vals[i].imag();
        }
        add_variable_space(stream, rng);
    }
}

template<class Stream_, typename Value_>
void format_array(Stream_& stream, size_t n, const std::vector<Value_>& vals) {
    std::mt19937_64 rng(n);

    stream << "%%MatrixMarket";
    add_at_least_one_variable_space(stream, rng);
    stream << "vector";
    add_at_least_one_variable_space(stream, rng);
    stream << "array";
    add_at_least_one_variable_space(stream, rng);
    add_field<Stream_, Value_>(stream);
    add_variable_space(stream, rng);
    add_at_least_one_newline(stream, rng);

    add_variable_space(stream, rng);
    stream << n;
    add_variable_space(stream, rng);

    for (size_t i = 0; i < vals.size(); ++i) {
        add_at_least_one_newline(stream, rng);
        add_variable_space(stream, rng);
        if constexpr(std::is_same<Value_, int>::value || std::is_same<Value_, double>::value) {
            stream << vals[i];
        } else if constexpr(std::is_same<Value_, std::complex<double> >::value) {
            stream << vals[i].real();
            add_at_least_one_variable_space(stream, rng);
            stream << vals[i].imag();
        }
        add_variable_space(stream, rng);
    }
}

#endif
