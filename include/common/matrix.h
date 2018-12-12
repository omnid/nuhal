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

/// @brief a 6x1 matrix
struct matrix_6x1
{
    float data[6];
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

/// @brief a 6x6 matrix
struct matrix_6x6
{
    // [row][col]
    float data[6][6];

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

/// @brief initialize a 6x6 matrix
/// @param out - the matrix to initialize
/// @param a11-a66 - the matrix entries
void matrix_6x6_init(struct matrix_6x6 * out,
                    float a11, float a12, float a13, float a14, float a15, float a16,
                    float a21, float a22, float a23, float a24, float a25, float a26,
                    float a31, float a32, float a33, float a34, float a35, float a36,
                    float a41, float a42, float a43, float a44, float a45, float a46,
                    float a51, float a52, float a53, float a54, float a55, float a56,
                    float a61, float a62, float a63, float a64, float a65, float a66);

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

/// @brief multiply a 6x6 matrix by a 6x1 vector
/// @param A - the matrix
/// @param x - the vector
/// @param v - the output vector v = Ax
void matrix_6x6_multiply_vector(const struct matrix_6x6 * A,
                                    const struct matrix_6x1 * x,
                                    struct matrix_6x1 * v);

/// @brief calculates the cross product of two 3x1 matrices
/// @param w - input matrix
/// @param v - input matrix
/// @param out - output matrix
void matrix_3x1_cross(const struct matrix_3x1 * w,
                      const struct matrix_3x1 * v,
                      struct matrix_3x1 * out);

/// @brief calculates the dot product of two 3x3 matrices
/// @param A - input matrix
/// @param x - input matrix
/// @param b - output matrix
void matrix_3x3_multiply_matrix(const struct matrix_3x3 * A,
                                const struct matrix_3x3 * x,
                                struct matrix_3x3 * v);

#ifdef __cplusplus
}
#endif

#endif
