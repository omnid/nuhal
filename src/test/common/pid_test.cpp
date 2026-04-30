/// \file
/// \brief test the PID controller implmeentation
#include "nuhal/catch.hpp"
#include "nuhal/pid.h"
#include "nuhal/bytestream.h"
#include "nuhal/utilities.h"

/// Test pid gain serialization
TEST_CASE("pid_serialize_gains", "[pid]")
{
    uint8_t buffer[sizeof(pid_gains)] = "";
    bytestream bs;
    bytestream_init(&bs, buffer, ARRAY_LEN(buffer));

    struct pid_gains gains{1.1f, 2.2f, 3.3f, 4.4f, 5.5f, 6.6f, 7.7f}; 
    pid_gains_inject(&bs, &gains);
    
    // reinitialize so we can read the values
    bytestream_init(&bs, buffer, ARRAY_LEN(buffer));

    struct pid_gains results{};
    pid_gains_extract(&bs, &results);
    CHECK(results.kp == gains.kp);
    CHECK(results.kd == gains.kd);
    CHECK(results.ki == gains.ki);
    CHECK(results.u_max == gains.u_max);
    CHECK(results.u_min == gains.u_min);
    CHECK(results.i_max == gains.i_max);
    CHECK(results.i_min == gains.i_min);
}

/// Test pid state serialization
TEST_CASE("pid_serialize_state", "[pid]")
{
    uint8_t buffer[sizeof(pid_state)] = "";
    bytestream bs;
    bytestream_init(&bs, buffer, ARRAY_LEN(buffer));

    struct pid_state state{2.1f, 3.2f, 4.3f};
    pid_state_inject(&bs, &state);
    
    // reinitialize so we can read the values
    bytestream_init(&bs, buffer, ARRAY_LEN(buffer));

    struct pid_state results{};
    pid_state_extract(&bs, &results);
    CHECK(results.p_error == state.p_error);
    CHECK(results.i_error == state.i_error);
    CHECK(results.d_error == state.d_error);
}

/// Test pid_signal serialization
TEST_CASE("pid_serialize_signals", "[pid]")
{
    uint8_t buffer[sizeof(pid_signals)] = "";
    bytestream bs;
    bytestream_init(&bs, buffer, ARRAY_LEN(buffer));
    struct pid_signals sig{1.5f, 4.9f, 7.2f};
    pid_signals_inject(&bs, &sig);

    // reinitialize so we can read the values
    bytestream_init(&bs, buffer, ARRAY_LEN(buffer));
    struct pid_signals results{};
    pid_signals_extract(&bs, &results);
    CHECK(results.reference == sig.reference);
    CHECK(results.measurement == sig.measurement);
    CHECK(results.effort == sig.effort);
}

/// Test pid_debug_info serialization
TEST_CASE("pid_serialize_debug_info", "[pid]")
{
    uint8_t buffer[sizeof(pid_debug_info)] = "";
    bytestream bs;
    bytestream_init(&bs, buffer, ARRAY_LEN(buffer));
    struct pid_debug_info info{{115.0f, 212.0, 1333.0f}, {11.1f, 22.2f, 33.3f}, 0, 0};
    pid_debug_info_inject(&bs, &info);

    // reinitialize so we can read the values
    bytestream_init(&bs, buffer, ARRAY_LEN(buffer));
    struct pid_debug_info results{};
    pid_debug_info_extract(&bs, &results);
    CHECK(results.signals.reference == info.signals.reference);
    CHECK(results.signals.measurement == info.signals.measurement);
    CHECK(results.signals.effort == info.signals.effort);
    CHECK(results.state.p_error == info.state.p_error);
    CHECK(results.state.i_error == info.state.i_error);
    CHECK(results.state.d_error == info.state.d_error);
}

/// Test some basic input/output relationships for the pid controller
/// TODO: Test antiwindup techniques
/// TODO: Test saturation
TEST_CASE("pid_basics", "[pid]")
{
    const struct pid_gains gains{4.0, 3.0, 2.0, 20.0, -20.0, 0.0, 0.0};

    struct pid_state state{0.0, 0.0, 0.0};

    const auto u1 = pid_compute(&gains, &state, 1.0, -0.5, SATURATE_NONE, ANTIWINDUP_NONE);
    CHECK(u1 == Approx(13.5));
    CHECK(state.p_error == Approx(1.5));
    CHECK(state.i_error == Approx(1.5));
    CHECK(state.d_error == Approx(1.5));

    const auto u2 = pid_compute(&gains, &state, 1.0, 1.2, SATURATE_NONE, ANTIWINDUP_NONE);
    CHECK(u2 == Approx(-0.3));
    CHECK(state.p_error == Approx(-0.2));
    CHECK(state.i_error == Approx(1.3));
    CHECK(state.d_error == Approx(-1.7)); 
}
