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

void matrix_6x6_init(struct matrix_6x6 * out,
                     float a11, float a12, float a13, float a14, float a15, float a16,
                     float a21, float a22, float a23, float a24, float a25, float a26,
                     float a31, float a32, float a33, float a34, float a35, float a36,
                     float a41, float a42, float a43, float a44, float a45, float a46,
                     float a51, float a52, float a53, float a54, float a55, float a56,
                     float a61, float a62, float a63, float a64, float a65, float a66)
{
    if(!out)
    {
        error(FILE_LINE, "NULL ptr");
    }

    out->data[0][0] = a11;
    out->data[0][1] = a12;
    out->data[0][2] = a13;
    out->data[0][3] = a14;
    out->data[0][4] = a15;
    out->data[0][5] = a16;

    out->data[1][0] = a21;
    out->data[1][1] = a22;
    out->data[1][2] = a23;
    out->data[1][3] = a24;
    out->data[1][4] = a25;
    out->data[1][5] = a26;

    out->data[2][0] = a31;
    out->data[2][1] = a32;
    out->data[2][2] = a33;
    out->data[2][3] = a34;
    out->data[2][4] = a35;
    out->data[2][5] = a36;

    out->data[3][0] = a41;
    out->data[3][1] = a42;
    out->data[3][2] = a43;
    out->data[3][3] = a44;
    out->data[3][4] = a45;
    out->data[3][5] = a46;

    out->data[4][0] = a51;
    out->data[4][1] = a52;
    out->data[4][2] = a53;
    out->data[4][3] = a54;
    out->data[4][4] = a55;
    out->data[4][5] = a56;

    out->data[5][0] = a61;
    out->data[5][1] = a62;
    out->data[5][2] = a63;
    out->data[5][3] = a64;
    out->data[5][4] = a65;
    out->data[5][5] = a66;
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

    // Matrix cannot be a transpose, otherwise multiplication will not work
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

void matrix_4x3T_4x1_multiply_vector(const struct matrix_4x3 * A,
                                     const struct matrix_4x1 * x,
                                     struct matrix_3x1 * v)
{
  if(!A || !x || !v)
  {
      error(FILE_LINE, "NULL ptr");
  }

  // Matrix must be a transpose, otherwise multiplication will not work
  if(!A->transpose)
  {
    error(FILE_LINE, "Invalid shape");
  }

  v->data[0] = A->data[0][0]*x->data[0] + A->data[1][0]*x->data[1]
      + A->data[2][0]*x->data[2] + A->data[3][0]*x->data[3];
  v->data[1] = A->data[0][1]*x->data[0] + A->data[1][1]*x->data[1]
      + A->data[2][1]*x->data[2] + A->data[3][1]*x->data[3];
  v->data[2] = A->data[0][2]*x->data[0] + A->data[1][2]*x->data[1]
      + A->data[2][2]*x->data[2] + A->data[3][2]*x->data[3];
}

void matrix_6x6_6x1_multiply_vector(const struct matrix_6x6 * A,
                                    const struct matrix_6x1 * x,
                                    struct matrix_6x1 * v)
{
    if(!A || !x || !v)
    {
        error(FILE_LINE, "NULL ptr");
    }

    v->data[0] = A->data[0][0]*x->data[0] + A->data[0][1]*x->data[1]
        + A->data[0][2]*x->data[2] + A->data[0][3]*x->data[3]
        + A->data[0][4]*x->data[4] + A->data[0][5]*x->data[5];
    v->data[1] = A->data[1][0]*x->data[0] + A->data[1][1]*x->data[1]
        + A->data[1][2]*x->data[2] + A->data[1][3]*x->data[3]
        + A->data[1][4]*x->data[4] + A->data[1][5]*x->data[5];
    v->data[2] = A->data[2][0]*x->data[0] + A->data[2][1]*x->data[1]
        + A->data[2][2]*x->data[2] + A->data[2][3]*x->data[3]
        + A->data[2][4]*x->data[4] + A->data[2][5]*x->data[5];
    v->data[3] = A->data[3][0]*x->data[0] + A->data[3][1]*x->data[1]
        + A->data[3][2]*x->data[2] + A->data[3][3]*x->data[3]
        + A->data[3][4]*x->data[4] + A->data[3][5]*x->data[5];
    v->data[4] = A->data[4][0]*x->data[0] + A->data[4][1]*x->data[1]
        + A->data[4][2]*x->data[2] + A->data[4][3]*x->data[3]
        + A->data[4][4]*x->data[4] + A->data[4][5]*x->data[5];
    v->data[5] = A->data[5][0]*x->data[0] + A->data[5][1]*x->data[1]
        + A->data[5][2]*x->data[2] + A->data[5][3]*x->data[3]
        + A->data[5][4]*x->data[4] + A->data[5][5]*x->data[5];
}

void matrix_3x1_cross(const struct matrix_3x1 * w,
                      const struct matrix_3x1 * v,
                      struct matrix_3x1 * out)
{
    if(!w || !v || !out)
    {
        error(FILE_LINE, "NULL ptr");
    }

    out->data[0] = (w->data[1] * v->data[2]) - (v->data[1] * w->data[2]);
    out->data[1] = (v->data[0] * w->data[2]) - (w->data[0] * v->data[2]);
    out->data[2] = (w->data[0] * v->data[1]) - (v->data[0] * w->data[1]);
}
