#include "nuhal/utilities.h"
#include "catch.hpp"

TEST_CASE("utilities_strlen", "[utilities]")
{
    CHECK(5 == utilities_strlen("12345", 10));
    CHECK(5 == utilities_strlen("12345", 5));
    CHECK(2 == utilities_strlen("12345", 2));
}
