#ifndef EMINEM_UTILS_HPP
#define EMINEM_UTILS_HPP

namespace eminem {

// See https://networkrepository.com/mtx-matrix-market-format.html for legal values.

enum class Object : char { MATRIX, VECTOR };

enum class Format : char { COORDINATE, ARRAY };

enum class Field : char { REAL, DOUBLE, COMPLEX, INTEGER, PATTERN };
    
enum class Symmetry : char { GENERAL, SYMMETRIC, SKEW_SYMMETRIC, HERMITIAN };

struct MatrixDetails {
    Object object;
    Format format;
    Field field;
    Symmetry symmetry;
};

}

#endif
