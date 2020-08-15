#ifndef NUHAL_MATRIX_INCLUDE_GUARD
#define NUHAL_MATRIX_INCLUDE_GUARD
/// @file
/// @brief Some basic matrix operations for use on the embedded systems.
///
/// Not intended as a general purpose matrix library. Rather, this contains
/// hard-coded matrix operations that are specifically needed by the
/// microcontroller to implement various robotic mathematical functions.
/// Features will be added as the omnid project requires.
#include<stdbool.h>

/// @brief a 3x3 matrix
struct matrix_3x3
{
    /// Data in row major order [row][col]
    float data[3][3];

    /// \brief If true the data should be treated as transposed.
    ///
    /// If true [col][row] indexing is used
    bool transpose;
};


/// @brief a 3x1 matrix (aka a vector)
struct matrix_3x1
{
    /// The elements of the vector
    float data[3];
};

/// @brief a 4x1 matrix
struct matrix_4x1
{
    /// The elements of the vector
    float data[4];
};

/// @brief a 6x1 matrix
struct matrix_6x1
{
    /// The elements of the vector
    float data[6];
};

/// @brief a 4x3 matrix
struct matrix_4x3
{
    /// Data in row major order [row][col]
    float data[4][3];

    /// \brief If true the data should be treated as transposed
    ///
    /// So if true column-major [col][row] indexing is used
    bool transpose;
};

/// @brief a 6x6 matrix
struct matrix_6x6
{
    /// Data in row-major order [row][col]
    float data[6][6];

    /// \brief If true the data should be treated as transposed
    ///
    /// So if true column major [col][row] indexing is used
    bool transpose;
};

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Initialize a 3x3 matrix
/// @param[out] out The matrix to initialize
/// @param a11 
/// @param a12 
/// @param a13 
/// @param a21 
/// @param a22 
/// @param a23 
/// @param a31 
/// @param a32 
/// @param a33 
void matrix_3x3_init(struct matrix_3x3 * out,
                     float a11, float a12, float a13,
                     float a21, float a22, float a23,
                     float a31, float a32, float a33);

/// @brief Initialize a 4x3 matrix
/// @param[out] out The matrix to initialize
/// @param a11 
/// @param a12 
/// @param a13 
/// @param a21 
/// @param a22 
/// @param a23 
/// @param a31 
/// @param a32 
/// @param a33 
/// @param a41 
/// @param a42 
/// @param a43 
void matrix_4x3_init(struct matrix_4x3 * out,
                     float a11, float a12, float a13,
                     float a21, float a22, float a23,
                     float a31, float a32, float a33,
                     float a41, float a42, float a43);

/// @brief Initialize a 6x6 matrix
/// @param a11 
/// @param a12 
/// @param a13 
/// @param a14 
/// @param a15 
/// @param a16 
/// @param a21 
/// @param a22 
/// @param a23 
/// @param a24 
/// @param a25 
/// @param a26 
/// @param a31 
/// @param a32 
/// @param a33 
/// @param a34 
/// @param a35 
/// @param a36 
/// @param a41 
/// @param a42 
/// @param a43 
/// @param a44 
/// @param a45 
/// @param a46 
/// @param a51 
/// @param a52 
/// @param a53 
/// @param a54 
/// @param a55 
/// @param a56 
/// @param a61 
/// @param a62 
/// @param a63 
/// @param a64 
/// @param a65 
/// @param a66 
/// @param[out] out The matrix to initialize
void matrix_6x6_init(struct matrix_6x6 * out,
                    float a11, float a12, float a13, float a14, float a15, float a16,
                    float a21, float a22, float a23, float a24, float a25, float a26,
                    float a31, float a32, float a33, float a34, float a35, float a36,
                    float a41, float a42, float a43, float a44, float a45, float a46,
                    float a51, float a52, float a53, float a54, float a55, float a56,
                    float a61, float a62, float a63, float a64, float a65, float a66);

/// @brief Transpose the 3x3 matrix
/// @param[in,out] mat The matrix to transpose
void matrix_3x3_transpose(struct matrix_3x3 * mat);

/// @brief Transposes a 3x3 matrix and returns it as a new matrix, with the data copied.
///
/// Instead of setting the transpose flag, the data is copied to the output matrix in transposed order
/// @param[in] mat  The matrix to transpose
/// @param[out] mat_tr - The transposed matrix
void matrix_3x3_return_transpose(struct matrix_3x3 * mat, struct matrix_3x3 * mat_tr);

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

/// @brief Calculates the dot product of two 3x3 matrices
/// @param A - input matrix
/// @param x - input matrix
/// @param[out] v - output matrix
void matrix_3x3_multiply_matrix(const struct matrix_3x3 * A,
                                const struct matrix_3x3 * x,
                                struct matrix_3x3 * v);

#ifdef __cplusplus
}
#endif

#endif
