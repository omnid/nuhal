#include "common/matrix.h"
#include "common/error.h"

void matrix_3x3_init(struct matrix_3x3 * out,
                     float a11, float a12, float a13,
                     float a21, float a22, float a23,
                     float a31, float a32, float a33)
{
    if(!out)
    {
        error(FILE_LINE, "NULL ptr");
    }

    out->data[0][0] = a11;
    out->data[0][1] = a12;
    out->data[0][2] = a13;

    out->data[1][0] = a21;
    out->data[1][1] = a22;
    out->data[1][2] = a23;


    out->data[2][0] = a31;
    out->data[2][1] = a32;
    out->data[2][2] = a33;
    out->transpose = false;
}

void matrix_4x3_init(struct matrix_4x3 * out,
                     float a11, float a12, float a13,
                     float a21, float a22, float a23,
                     float a31, float a32, float a33,
                     float a41, float a42, float a43)
{
    if(!out)
    {
        error(FILE_LINE, "NULL ptr");
    }

    out->data[0][0] = a11;
    out->data[0][1] = a12;
    out->data[0][2] = a13;

    out->data[1][0] = a21;
    out->data[1][1] = a22;
    out->data[1][2] = a23;

    out->data[2][0] = a31;
    out->data[2][1] = a32;
    out->data[2][2] = a33;

    out->data[3][0] = a41;
    out->data[3][1] = a42;
    out->data[3][2] = a43;
    out->transpose = false;
}

void matrix_3x3_transpose(struct matrix_3x3 * mat)
{
    if(!mat)
    {
        error(FILE_LINE, "NULL ptr");
    }
    mat->transpose = !mat->transpose;
}

void matrix_4x3_transpose(struct matrix_4x3 * mat)
{
    if(!mat)
    {
        error(FILE_LINE, "NULL ptr");
    }
    mat->transpose = !mat->transpose;
}

void  matrix_3x3_multiply_vector(const struct matrix_3x3 * A,
                                 const struct matrix_3x1 * x,
                                 struct matrix_3x1 * v)
{
    if(!A || !x || !v)
    {
        error(FILE_LINE, "NULL ptr");
    }

    if(A->transpose)
    {
        v->data[0] = A->data[0][0]*x->data[0] + A->data[1][0]*x->data[1]
            + A->data[2][0]*x->data[2];
        v->data[1] = A->data[0][1]*x->data[0] + A->data[1][1]*x->data[1]
            + A->data[2][1]*x->data[2];
        v->data[2] = A->data[0][2]*x->data[0] + A->data[1][2]*x->data[1]
            + A->data[2][2]*x->data[2];
    }
    else
    {
        v->data[0] = A->data[0][0]*x->data[0] + A->data[0][1]*x->data[1]
            + A->data[0][2]*x->data[2];
        v->data[1] = A->data[1][0]*x->data[0] + A->data[1][1]*x->data[1]
            + A->data[1][2]*x->data[2];
        v->data[2] = A->data[2][0]*x->data[0] + A->data[2][1]*x->data[1]
            + A->data[2][2]*x->data[2];
    }
}

void matrix_4x3_3x1_multiply_vector(const struct matrix_4x3 * A,
                                    const struct matrix_3x1 * x,
                                    struct matrix_4x1 * v)
{
    if(!A || !x || !v)
    {
        error(FILE_LINE, "NULL ptr");
    }

    if(A->transpose)
    {
        error(FILE_LINE, "Invalid shape");
    }

    v->data[0] = A->data[0][0]*x->data[0] + A->data[0][1]*x->data[1]
        + A->data[0][2]*x->data[2];
    v->data[1] = A->data[1][0]*x->data[0] + A->data[1][1]*x->data[1]
        + A->data[1][2]*x->data[2];
    v->data[2] = A->data[2][0]*x->data[0] + A->data[2][1]*x->data[1]
        + A->data[2][2]*x->data[2];
    v->data[3] = A->data[3][0]*x->data[0] + A->data[3][1]*x->data[1]
        + A->data[3][2]*x->data[2];
}
