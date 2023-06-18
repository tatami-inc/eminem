#ifndef EMINEM_TEST_SIMULATE_H 
#define EMINEM_TEST_SIMULATE_H

#include <vector>
#include <type_traits>
#include <random>
#include <complex>
#include <iomanip>

template<class Stream_, typename Value_>
void format_coordinates(Stream_& stream, size_t nr, size_t nc, const std::vector<int>& rows, const std::vector<int>& cols, const std::vector<Value_>& vals) {
    stream << "%%MatrixMarket matrix coordinate ";
    if constexpr(std::is_same<Value_, int>::value) {
        stream << "integer";
    } else if constexpr(std::is_same<Value_, double>::value) {
        stream << "double";
        stream << std::setprecision(10);
    } else if constexpr(std::is_same<Value_, std::complex<double> >::value) {
        stream << "complex";
    } else {
        stream << "pattern";
    }
    stream << " general\n";

    stream << nr << " " << nc << " " << rows.size();

    for (size_t i = 0; i < rows.size(); ++i) {
        stream << "\n" << rows[i] + 1 << " " << cols[i] + 1;
        if constexpr(std::is_same<Value_, int>::value || std::is_same<Value_, double>::value) {
            stream << " " << vals[i];
        } else if constexpr(std::is_same<Value_, std::complex<double> >::value) {
            stream << " " << vals[i].real << " " << vals[i].imag;
        }
    }

    return;
}

inline std::pair<std::vector<int>, std::vector<int> > simulate_coordinates(size_t nr, size_t nc, double density) {
    std::vector<int> rows, columns;
    std::mt19937_64 rng(std::round(static_cast<double>(nr * nc) / density));
    std::uniform_real_distribution dist;

    for (size_t r = 0; r < nr; ++r) {
        for (size_t c = 0; c < nc; ++c) {
            if (dist(rng) <= density) {
                rows.push_back(r);
                columns.push_back(c);
            }
        }
    }

    return std::make_pair(std::move(rows), std::move(columns));
}

inline std::vector<int> simulate_integers(size_t n, int lower, int upper) {
    int range = upper - lower;
    std::mt19937_64 rng(std::round(n * range));

    std::vector<int> values;
    for (size_t i = 0; i < n; ++i) {
        values.push_back(rng() % range + lower);
    }

    return values;
}

inline std::vector<double> simulate_real(size_t n) {
    std::mt19937_64 rng(n);
    std::normal_distribution ndist;

    std::vector<double> values;
    for (size_t i = 0; i < n; ++i) {
        values.push_back(ndist(rng));
    }

    return values;
}

#endif
