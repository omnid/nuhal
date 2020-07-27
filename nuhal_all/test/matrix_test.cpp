#include "nuhal/catch.hpp"
#include "nuhal/matrix.h"

TEST_CASE("matrix_multiply", "[matrix]")
{
    // 3x3 matrix multiplication
    struct matrix_3x3 A;
    matrix_3x3_init(&A,
                1, 2, 3,
                4, 5, 6,
                7, 8, 9);
    struct matrix_3x1 x = {{1, 2, 3}};
    struct matrix_3x1 b = {{0, 0, 0}};
    matrix_3x3_multiply_vector(&A, &x, &b);
    CHECK(b.data[0] == 1 + 4 + 9);
    CHECK(b.data[1] == 4 + 10 + 18);
    CHECK(b.data[2] == 7 + 16 + 27);
}

TEST_CASE("matrix_multiply_4x3_3x1", "[matrix]")
{
  // 4x3 matrix multiplication
  struct matrix_4x3 A;
  matrix_4x3_init(&A,
              1, 2, 3,
              4, 5, 6,
              7, 8, 9,
              10, 11, 12);
  struct matrix_3x1 x = {{1, 2, 3}};
  struct matrix_4x1 b = {{0, 0, 0, 0}};
  matrix_4x3_3x1_multiply_vector(&A, &x, &b);
  CHECK(b.data[0] == 1 + 4 + 9);
  CHECK(b.data[1] == 4 + 10 + 18);
  CHECK(b.data[2] == 7 + 16 + 27);
  CHECK(b.data[3] == 10 + 22 + 36);
}

TEST_CASE("matrix_multiply_3x4_4x1", "[matrix]")
{
  // Multiply a 3x4 matrix by a 4x1 vector
  struct matrix_4x3 A;
  matrix_4x3_init(&A,
              1, 2, 3,
              4, 5, 6,
              7, 8, 9,
              10, 11, 12);
  A.transpose = true;
  struct matrix_3x1 b = {{0, 0, 0}};
  struct matrix_4x1 x = {{5, 5, 5, 5}};
  matrix_4x3T_4x1_multiply_vector(&A, &x, &b);
  CHECK(b.data[0] == 5 + 20 + 35 + 50);
  CHECK(b.data[1] == 10 + 25 + 40 + 55);
  CHECK(b.data[2] == 15 + 30 + 45 + 60);
}

TEST_CASE("matrix_multiply_6x6_6x1", "[matrix]")
{
    struct matrix_6x6 A;
    matrix_6x6_init(&A,
            1, 2, 3, 4, 5, 6,
            7, 8, 9, 10, 11, 12,
            13, 14, 15, 16, 17, 18,
            19, 20, 21, 22, 23, 24,
            25, 26, 27, 28, 29, 30,
            31, 32, 33, 34, 35, 36);
    struct matrix_6x1 x = {{1, 2, 3, 4, 5, 6}};
    struct matrix_6x1 b = {{0, 0, 0, 0, 0, 0}};
    matrix_6x6_multiply_vector(&A, &x, &b);
    CHECK(b.data[0] == 91);
    CHECK(b.data[1] == 217);
    CHECK(b.data[2] == 343);
    CHECK(b.data[3] == 469);
    CHECK(b.data[4] == 595);
    CHECK(b.data[5] == 721);
}

TEST_CASE("matrix_3x1_cross_product", "[matrix]")
{
    struct matrix_3x1 r = {{1, 2, 3}};
    struct matrix_3x1 w = {{4, 5, 6}};
    struct matrix_3x1 v = {{0, 0, 0}};

    matrix_3x1_cross(&r, &w, &v);
    CHECK(v.data[0] == -3);
    CHECK(v.data[1] == 6);
    CHECK(v.data[2] == -3);
}

TEST_CASE("matrix_multiply_3x3_3x3", "[matrix]")
{
    struct matrix_3x3 A;
    matrix_3x3_init(&A,
                    1, 2, 3,
                    4, 5, 6,
                    7, 8, 9);
    struct matrix_3x3 x;
    matrix_3x3_init(&x,
                    10, 11, 12,
                    13, 14, 15,
                    16, 17, 18);
    struct matrix_3x3 b;
    matrix_3x3_init(&b,
                    0, 0, 0,
                    0, 0, 0,
                    0, 0, 0);
    matrix_3x3_multiply_matrix(&A, &x, &b);
    CHECK(b.data[0][0] == 84);
    CHECK(b.data[0][1] == 90);
    CHECK(b.data[0][2] == 96);

    CHECK(b.data[1][0] == 201);
    CHECK(b.data[1][1] == 216);
    CHECK(b.data[1][2] == 231);

    CHECK(b.data[2][0] == 318);
    CHECK(b.data[2][1] == 342);
    CHECK(b.data[2][2] == 366);
}
