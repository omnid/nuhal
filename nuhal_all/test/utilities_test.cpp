#include "nuhal/utilities.h"
#include "nuhal/catch.hpp"

TEST_CASE("utilities_strlen", "[utilities]")
{
    CHECK(5 == utilities_strlen("12345", 10));
    CHECK(5 == utilities_strlen("12345", 5));
    CHECK(2 == utilities_strlen("12345", 2));
}

TEST_CASE("utilities_normalize_angle", "[utilities]")
{
    CHECK(0.0f == Approx( normalize_angle(0.0f) ));
    CHECK(PI == Approx( normalize_angle(PI) ));
    CHECK(-PI == Approx( normalize_angle(-PI) ));

    CHECK(0.0f == Approx( normalize_angle(2.0f * PI) ));
    CHECK(0.0f == Approx( normalize_angle(-2.0f * PI) ));
    CHECK(0.0f == Approx( normalize_angle(4.0f * PI) ));
    CHECK(0.0f == Approx( normalize_angle(-4.0f * PI) ));


    CHECK(-0.5f * PI == Approx( normalize_angle(1.5f * PI) ));
    CHECK(0.5f * PI == Approx( normalize_angle(-1.5f * PI) ));
    CHECK(0.5f * PI == Approx( normalize_angle(2.5f * PI) ));
    CHECK(-0.5f * PI == Approx( normalize_angle(-2.5f * PI) ));
}
