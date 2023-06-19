#ifndef EMINEM_TEST_SIMULATE_H 
#define EMINEM_TEST_SIMULATE_H

#include <vector>
#include <random>
#include <complex>
#include <iomanip>

inline std::pair<std::vector<int>, std::vector<int> > simulate_coordinate(size_t nr, size_t nc, double density) {
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

inline std::vector<int> simulate_coordinate(size_t n, double density) {
    std::vector<int> entries;
    std::mt19937_64 rng(std::round(static_cast<double>(n) / density));
    std::uniform_real_distribution dist;

    for (size_t i = 0; i < n; ++i) {
        if (dist(rng) <= density) {
            entries.push_back(i);
        }
    }

    return entries;
}

inline std::vector<int> simulate_integer(size_t n, int lower, int upper) {
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

inline std::vector<std::complex<double> > simulate_complex(size_t n) {
    std::mt19937_64 rng(n);
    std::normal_distribution ndist;

    std::vector<std::complex<double> > values;
    for (size_t i = 0; i < n; ++i) {
        values.emplace_back(ndist(rng), ndist(rng));
    }

    return values;
}


#endif
