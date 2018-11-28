#ifndef COMMON_MATRIX_INCLUDE_GUARD
#define COMMON_MATRIX_INCLUDE_GUARD
/// @brief some basic matrix operations for use on the embedded systems
/// functions will be added as needed
#include<stdbool.h>

/// @brief a 3x3 matrix
struct matrix_3x3
{
    // [row][col]
    float data[3][3];
    // if true the data should be treated as transposed
    // so if true [col][row] indexing is used
    bool transpose;
};


/// @brief a 3x1 matrix (aka a vector)
struct matrix_3x1
{
    float data[3];
};

/// @brief a 4x1 matrix
struct matrix_4x1
{
    float data[4];
};

/// @brief a 4x3 matrix
struct matrix_4x3
{
    // [row][col]
    float data[4][3];

    // if true the data should be treated as transposed
    // so if true [col][row] indexing is used
    bool transpose;
};

#ifdef __cplusplus
extern "C" {
#endif

/// @brief initialize a 3x3 matrix
/// @param out - the matrix to initialize
/// @param a11-a33 - the matrix entries
void matrix_3x3_init(struct matrix_3x3 * out,
                     float a11, float a12, float a13,
                     float a21, float a22, float a23,
                     float a31, float a32, float a33);

/// @brief initialize a 4x3 matrix
/// @param out - the matrix to initialize
/// @param a11-a43 - the matrix entries
void matrix_4x3_init(struct matrix_4x3 * out,
                     float a11, float a12, float a13,
                     float a21, float a22, float a23,
                     float a31, float a32, float a33,
                     float a41, float a42, float a43);

/// @brief transpose the 3x3 matrix
/// @param mat - the matrix to transpose
void matrix_3x3_transpose(struct matrix_3x3 * mat);

/// @brief transpose the 4x3 matrix
/// @param mat - the matrix to transpose
void matrix_4x3_transpose(struct matrix_4x3 * mat);

/// @brief multiply a 3x3 matrix by a 3x1 matrix
/// @param A - the matrix
/// @param x  - the vector
/// @param v - the output vector v = Ax
void  matrix_3x3_multiply_vector(const struct matrix_3x3 * A,
                                 const struct matrix_3x1 * x,
                                 struct matrix_3x1 * v);

/// @brief multiply a 4x3 matrix by a 3x1 matrix
/// @param A - the matrix
/// @param x  - the vector
/// @param v - the output vector v = Ax
void matrix_4x3_3x1_multiply_vector(const struct matrix_4x3 * A,
                                    const struct matrix_3x1 * x,
                                    struct matrix_4x1 * v);

/// @brief multiply a 3x4 matrix by a 4x1 vector
/// @param A - the matrix (uses the 4x3 matrix struct with .transpose = true)
/// @param x - the vector
/// @param v - the output vector v = Ax
void matrix_4x3T_4x1_multiply_vector(const struct matrix_4x3 * A,
                                     const struct matrix_4x1 * x,
                                     struct matrix_3x1 * v);

#ifdef __cplusplus
}
#endif

#endif
