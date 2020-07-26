#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "nuhal/bytestream.h"
#include "nuhal/utilities.h"
#include <limits>

/// @brief test bytestream injection and extraction

// test uint8_t
TEST_CASE("bytestream_u8", "[bytestream]")
{
    uint8_t buffer[3] = "";
    bytestream bs;
    bytestream_init(&bs, buffer, ARRAY_LEN(buffer));

    // inject some u8 values into the bytestream
    bytestream_inject_u8(&bs, 0);
    bytestream_inject_u8(&bs, 5);
    bytestream_inject_u8(&bs, 2);

    // reinitialize so we can read the values
    bytestream_init(&bs, buffer, ARRAY_LEN(buffer));
    CHECK(0 == bytestream_extract_u8(&bs));
    CHECK(5 == bytestream_extract_u8(&bs));
    CHECK(2 == bytestream_extract_u8(&bs));
}

// test uint16_t
TEST_CASE("bytestream_16", "[bytestream]")
{
    uint8_t buffer[sizeof(uint16_t) * 4] = "";
    bytestream bs;
    bytestream_init(&bs, buffer, ARRAY_LEN(buffer));

    // inject some u16 values and check that we can extract them
    bytestream_inject_u16(&bs, 20);
    bytestream_inject_u16(&bs, 1776);
    bytestream_inject_u16(&bs, 0);
    bytestream_inject_u16(&bs, 3402);

    bytestream_init(&bs, buffer, ARRAY_LEN(buffer));
    CHECK(20 == bytestream_extract_u16(&bs));
    CHECK(1776 == bytestream_extract_u16(&bs));
    CHECK(0 == bytestream_extract_u16(&bs));
    CHECK(3402 == bytestream_extract_u16(&bs));
}

// test uint32_t
TEST_CASE("bytestream_u32", "[bytestream]")
{
    uint8_t buffer[sizeof(uint32_t) * 4] = "";
    bytestream bs;
    bytestream_init(&bs, buffer, ARRAY_LEN(buffer));

    bytestream_inject_u32(&bs, 0xDEADBEAF);
    bytestream_inject_u32(&bs, 0x13371337);
    bytestream_inject_u32(&bs, 0);
    bytestream_inject_u32(&bs, 0x00009012);
    bytestream_init(&bs, buffer, ARRAY_LEN(buffer));
    CHECK(0xDEADBEAF == bytestream_extract_u32(&bs));
    CHECK(0x13371337 == bytestream_extract_u32(&bs));
    CHECK(         0 == bytestream_extract_u32(&bs));
    CHECK(0x00009012 == bytestream_extract_u32(&bs));
}

// test floats
TEST_CASE("bytestream_f", "[bytestream]")
{
    uint8_t buffer[sizeof(float) * 4] = "";
    bytestream bs;
    bytestream_init(&bs, buffer, ARRAY_LEN(buffer));
    bytestream_inject_f(&bs, 3.1415926f);
    bytestream_inject_f(&bs, std::numeric_limits<float>::quiet_NaN());
    bytestream_inject_f(&bs, -0.1234f);
    bytestream_inject_f(&bs, std::numeric_limits<float>::infinity());

    bytestream_init(&bs, buffer, ARRAY_LEN(buffer));
    CHECK(3.1415926f == bytestream_extract_f(&bs));
    CHECK(std::isnan(bytestream_extract_f(&bs)));
    CHECK(-0.1234f == bytestream_extract_f(&bs));
    CHECK(std::numeric_limits<float>::infinity() == bytestream_extract_f(&bs));
}

TEST_CASE("bytestream_string", "[bytestream]")
{
    uint8_t buffer[50] = "";
    bytestream bs;
    bytestream_init(&bs, buffer, ARRAY_LEN(buffer));

    bytestream_inject_string(&bs, "this is str 1");
    bytestream_inject_string(&bs, "number two");
    bytestream_inject_string(&bs, "hello\r\n");
    bytestream_init(&bs, buffer, ARRAY_LEN(buffer));

    char str[50] = "";
    bytestream_extract_string(&bs, str, ARRAY_LEN(str));
    CHECK(std::string(str) == "this is str 1");
    bytestream_extract_string(&bs, str, ARRAY_LEN(str));
    CHECK(std::string(str) == "number two");
    bytestream_extract_string(&bs, str, ARRAY_LEN(str));
    CHECK(std::string(str) == "hello\r\n");
}
