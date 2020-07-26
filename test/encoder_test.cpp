#include "catch.hpp"
#include "nuhal/encoder.h"
#include "nuhal/utilities.h"

TEST_CASE("encoder_ticks", "[encoder]")
{
    const struct encoder up{360, 100, ENCODER_ORIENTATION_UP_UP};

    struct encoder_raw ticks {0, 100};
    
    CHECK(encoder_ticks(&up, ticks) == 0);

    ticks.single = 120;
    CHECK(encoder_ticks(&up, ticks) == 20);

    ticks.single = 90;
    CHECK(encoder_ticks(&up, ticks)  == -10);
}

TEST_CASE("encoder_ticks_up_down", "[encoder]")
{
    const struct encoder down{360, 100, ENCODER_ORIENTATION_UP_DOWN};

    struct encoder_raw ticks {0, 100};
    CHECK(encoder_ticks(&down, ticks) == 0);

    ticks.single = 120;
    CHECK(encoder_ticks(&down, ticks) == -20);

    ticks.single = 90;
    CHECK(encoder_ticks(&down, ticks)  == 10);
}

TEST_CASE("encoder_radians_up_up", "[encoder]")
{
    const struct encoder enc{180, 90, ENCODER_ORIENTATION_UP_UP};

    CHECK(encoder_radians(&enc, 0) == Approx(0.0f));

    CHECK(encoder_radians(&enc, 10) == Approx(DEGREES_TO_RADIANS(20.0f)));

    CHECK(encoder_radians(&enc, -20) == Approx(DEGREES_TO_RADIANS(-40.0f)));
}
TEST_CASE("encoder_radians_up_down", "[encoder]")
{
    const struct encoder enc{180, 90, ENCODER_ORIENTATION_UP_DOWN};
    CHECK(encoder_radians(&enc, 0) == Approx(0.0f));

    CHECK(encoder_radians(&enc, -10) == Approx(DEGREES_TO_RADIANS(-20.0f)));

    CHECK(encoder_radians(&enc, 20) == Approx(DEGREES_TO_RADIANS(40.0f)));
}

TEST_CASE("encoder_zero_raw_up_up", "[encoder]")
{
    const struct encoder enc{360, 0, ENCODER_ORIENTATION_UP_UP};

    // given that a raw count of 120 corresponds to Pi radians
    // what count corresponds to zero radians?
    const struct encoder_raw count1{0, 120};
    CHECK(300 == encoder_zero_raw(&enc, count1, PI));

    // given that a raw count of 50 corresponds to Pi/2 radians
    // what count corresponds to zero radians?
    const struct encoder_raw count2{0, 50};
    CHECK(320 == encoder_zero_raw(&enc, count2, PI/2.0f));

    // given that a raw count of 240 corresponds to PI/4 radians
    // what count corresponds to zero radians
    const struct encoder_raw count3{0, 245};
    CHECK(200 == encoder_zero_raw(&enc, count3, PI/4.0f));


    const struct encoder_raw count4{0, 245};
    CHECK(275 == encoder_zero_raw(&enc, count4, -PI/6.0f));
}


TEST_CASE("encoder_zero_raw_up_down", "[encoder]")
{
    // here increasing counts correspond to decreasing angles
    const struct encoder enc{360, 0, ENCODER_ORIENTATION_UP_DOWN};

    // given that a raw count of 120 corresponds to Pi radians
    // what count corresponds to zero radians?
    const struct encoder_raw count1{0, 120};
    CHECK(300 == encoder_zero_raw(&enc, count1, PI));

    // given that a raw count of 50 corresponds to Pi/2 radians
    // what count corresponds to zero radians?
    const struct encoder_raw count2{0, 50};
    CHECK(140 == encoder_zero_raw(&enc, count2, PI/2.0f));

    // given that a raw count of 240 corresponds to PI/4 radians
    // what count corresponds to zero radians
    const struct encoder_raw count3{0, 245};
    CHECK(290 == encoder_zero_raw(&enc, count3, PI/4.0f));

    const struct encoder_raw count4{0, 245};
    CHECK(215 == encoder_zero_raw(&enc, count4, -PI/6.0f));
}
