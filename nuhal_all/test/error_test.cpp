#include "nuhal/error.h"
#include "nuhal/catch.hpp"
/// Test the error handling code, taking advantage that the error stub functions throw an exception

TEST_CASE("Basic Error", "[error]")
{
    CHECK_THROWS_WITH(error("FILE:LINE", "This Is A Test Error Message"),
                      "FILE:LINE: This Is A Test Error Message");
    CHECK_THROWS_WITH(error("FILE:LINE", "Hello %d %c %s", 5, 'A', "Stuff"),
                      "FILE:LINE: (fatal) Hello 5 A Stuff");
}

