#include "nuhal/error.h"
#include "nuhal/catch.hpp"
/// Test the error handling code, taking advantage that the error stub functions throw an exception
/// The exception contains the error message, so we can use CHECK_THROWS_WITH to examine the error
/// message that is generated
TEST_CASE("Basic Error", "[error]")
{
    TEST_set_error_state(false);
    CHECK_THROWS_WITH(error("","%s",""), ": ");

    TEST_set_error_state(false);
    CHECK_THROWS_WITH(error("FILE:LINE", "This Is A Test Error Message"),
                      "FILE:LINE: This Is A Test Error Message");
    CHECK_THROWS_WITH(error("FILE:LINE", "Hello %d %c %s", 5, 'A', "Stuff"),
                      "FILE:LINE: (fatal) Hello 5 A Stuff");
}

TEST_CASE("Long Error", "[error]")
{
    TEST_set_error_state(false);
    const std::string msg(10, 'B');
    CHECK_THROWS_WITH(error("FILE:LINE", "%s", msg.c_str()), "FILE:LINE: " + msg);

}
