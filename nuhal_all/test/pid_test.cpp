/// \file
/// \brief test the PID controller implmeentation
#include "nuhal/catch.hpp"
#include "nuhal/pid.h"
#include "nuhal/bytestream.h"
#include "nuhal/utilities.h"


TEST_CASE("pid_serialize_gains", "[pid]")
{
    uint8_t buffer[sizeof(pid_gains)] = "";
    bytestream bs;
    bytestream_init(&bs, buffer, ARRAY_LEN(buffer));

    struct pid_gains gains{1.1f, 2.2f, 3.3f, 4.4f, 5.5f}; 
    pid_inject_gains(&bs, &gains);
    
    // reinitialize so we can read the values
    bytestream_init(&bs, buffer, ARRAY_LEN(buffer));

    struct pid_gains results{};
    pid_extract_gains(&bs, &results);
    CHECK(results.kp == gains.kp);
    CHECK(results.kd == gains.kd);
    CHECK(results.ki == gains.ki);
    CHECK(results.u_max == gains.u_max);
    CHECK(results.u_min == gains.u_min);
}

TEST_CASE("pid_serialize_state", "[pid]")
{
    uint8_t buffer[sizeof(pid_state)] = "";
    bytestream bs;
    bytestream_init(&bs, buffer, ARRAY_LEN(buffer));

    struct pid_state state{2.1f, 3.2f, 4.3f};
    pid_inject_state(&bs, &state);
    
    // reinitialize so we can read the values
    bytestream_init(&bs, buffer, ARRAY_LEN(buffer));

    struct pid_state results{};
    pid_extract_state(&bs, &results);
    CHECK(results.p_error == state.p_error);
    CHECK(results.i_error == state.i_error);
    CHECK(results.d_error == state.d_error);
}

TEST_CASE("pid_basics", "[pid]")
{
    const struct pid_gains gains{4.0, 3.0, 2.0, 20.0, -20.0};

    struct pid_state state{0.0, 0.0, 0.0};

    const auto u1 = pid_compute(&gains, &state, 1.0, -0.5);
    CHECK(u1 == Approx(13.5));
    CHECK(state.p_error == Approx(1.5));
    CHECK(state.i_error == Approx(1.5));
    CHECK(state.d_error == Approx(1.5));

    const auto u2 = pid_compute(&gains, &state, 1.0, 1.2);
    CHECK(u2 == Approx(-0.3));
    CHECK(state.p_error == Approx(-0.2));
    CHECK(state.i_error == Approx(1.3));
    CHECK(state.d_error == Approx(-1.7)); 
}
