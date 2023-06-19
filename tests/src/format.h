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

template<class Stream_, typename Value_>
void format_coordinate(Stream_& stream, size_t nr, size_t nc, const std::vector<int>& rows, const std::vector<int>& cols, const std::vector<Value_>& vals) {
    stream << "%%MatrixMarket matrix coordinate ";
    add_field<Stream_, Value_>(stream);
    stream << " general\n";

    stream << nr << " " << nc << " " << rows.size();

    for (size_t i = 0; i < rows.size(); ++i) {
        stream << "\n" << rows[i] + 1 << " " << cols[i] + 1;
        if constexpr(std::is_same<Value_, int>::value || std::is_same<Value_, double>::value) {
            stream << " " << vals[i];
        } else if constexpr(std::is_same<Value_, std::complex<double> >::value) {
            stream << " " << vals[i].real() << " " << vals[i].imag();
        }
    }
}

template<class Stream_, typename Value_>
void format_coordinate(Stream_& stream, size_t n, const std::vector<int>& entries, const std::vector<Value_>& vals) {
    stream << "%%MatrixMarket vector coordinate ";
    add_field<Stream_, Value_>(stream);
    stream << " general\n";

    stream << n << " " << entries.size();

    for (size_t i = 0; i < entries.size(); ++i) {
        stream << "\n" << entries[i] + 1;
        if constexpr(std::is_same<Value_, int>::value || std::is_same<Value_, double>::value) {
            stream << " " << vals[i];
        } else if constexpr(std::is_same<Value_, std::complex<double> >::value) {
            stream << " " << vals[i].real() << " " << vals[i].imag();
        }
    }
}

template<class Stream_, typename Value_>
void format_array(Stream_& stream, size_t nr, size_t nc, const std::vector<Value_>& vals) {
    stream << "%%MatrixMarket matrix array ";
    add_field<Stream_, Value_>(stream);
    stream << " general\n";

    stream << nr << " " << nc;

    for (size_t i = 0; i < vals.size(); ++i) {
        stream << "\n";
        if constexpr(std::is_same<Value_, int>::value || std::is_same<Value_, double>::value) {
            stream << vals[i];
        } else if constexpr(std::is_same<Value_, std::complex<double> >::value) {
            stream << vals[i].real() << " " << vals[i].imag();
        }
    }
}

template<class Stream_, typename Value_>
void format_array(Stream_& stream, size_t n, const std::vector<Value_>& vals) {
    stream << "%%MatrixMarket vector array ";
    add_field<Stream_, Value_>(stream);
    stream << " general\n";

    stream << n;

    for (size_t i = 0; i < vals.size(); ++i) {
        stream << "\n";
        if constexpr(std::is_same<Value_, int>::value || std::is_same<Value_, double>::value) {
            stream << vals[i];
        } else if constexpr(std::is_same<Value_, std::complex<double> >::value) {
            stream << vals[i].real() << " " << vals[i].imag();
        }
    }
}

#endif
