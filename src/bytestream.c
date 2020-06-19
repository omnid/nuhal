#include "nutilities/bytestream.h"
#include "nutilities/error.h"
#include <string.h>
// ensure that we can use floats and u32's in memory the same via unions
STATIC_ASSERT(sizeof(float) == sizeof(uint32_t), sizeof_float_u32);


// verify that the arguments are correct and cause an error if they are not
static void verify_args(const struct bytestream * bs, size_t len)
{
    if(!bs || !bs->data)
    {
        error(FILE_LINE, "NULL ptr");
    }
    // there are not enough bytes
    if(bs->size + len > bs->capacity)
    {
        error(FILE_LINE, "overflow");
    }
}

void bytestream_init(struct bytestream * out, uint8_t * data, size_t len)
{
    if(!data || !out)
    {
        error(FILE_LINE, "NULL ptr");
    }
    out->data = data;
    out->capacity = len;
    out->size = 0;
}

void bytestream_inject_u8(struct bytestream * bs, uint8_t u8)
{
    verify_args(bs, sizeof(u8));
    bs->data[bs->size] = u8;
    ++bs->size;
}

uint8_t bytestream_extract_u8(struct bytestream * bs)
{
    verify_args(bs, sizeof(uint8_t));
    const size_t index = bs->size;
    ++bs->size;
    return bs->data[index];
}

/// @brief convert the next data in the stream to a u16
/// @param bs - the bytestream, which will be advanced by two bytes
/// @return the uint16 in the bytestream
uint16_t bytestream_extract_u16(struct bytestream * bs)
{
    verify_args(bs, sizeof(uint16_t));

    const uint16_t res =
        (uint16_t)bs->data[bs->size + 1] | ((uint16_t)bs->data[bs->size] << 8);
    bs->size += sizeof(uint16_t);
    return res;
}

/// @brief place a uint16_t into the bytestream
/// @param bs - the bytestream, which will be advanced by two bytes
/// @param u16 - the data to place in the stream, in little endian format
void bytestream_inject_u16(struct bytestream * bs, uint16_t u16)
{
    verify_args(bs, sizeof(u16));

    // big endian on the bytestream side
    bs->data[bs->size + 1] = u16 & 0x00FF;
    bs->data[bs->size] = (u16 & 0xFF00) >> 8;
    bs->size += sizeof(uint16_t);
}


uint32_t bytestream_extract_u32(struct bytestream * bs)
{
    verify_args(bs, sizeof(uint32_t));

    // big endian on the bytestream side
    const uint32_t res =
        (uint32_t)bs->data[bs->size + 3]
        | ((uint32_t)bs->data[bs->size + 2] << 8)
        | ((uint32_t)bs->data[bs->size + 1] << 16)
        | ((uint32_t)bs->data[bs->size] << 24);
    bs->size += sizeof(uint32_t);
    return res;
}

void bytestream_inject_u32(struct bytestream * bs, uint32_t u32)
{
    verify_args(bs, sizeof(uint32_t));
    bs->data[bs->size + 3] = u32 & 0x000000FF;
    bs->data[bs->size + 2] = (u32 & 0x0000FF00) >> 8;
    bs->data[bs->size + 1] = (u32 & 0x00FF0000) >> 16;
    bs->data[bs->size] = (u32 & 0xFF000000) >> 24;
    bs->size += sizeof(uint32_t);
}

/// @brief get int32_t from the bytestream
/// @param bs - the bytestream, which will be advanced by two bytes
/// @return the next int32 from the stream
int32_t bytestream_extract_i32(struct bytestream * bs)
{
    return (int32_t)bytestream_extract_u32(bs);
}

/// @brief place a uint16_t into the bytestream
/// @param bs - the bytestream, which will be advanced by two bytes
/// @param i32 - the data to place in the stream, in little endian format
void bytestream_inject_i32(struct bytestream * bs, int32_t i32)
{
    bytestream_inject_u32(bs, (uint32_t)i32);
}

float bytestream_extract_f(struct bytestream * bs)
{
    verify_args(bs, sizeof(float));
    uint32_t float_as_u32 = bytestream_extract_u32(bs);
    float float_as_float = 0.0;
    memcpy(&float_as_float, &float_as_u32, sizeof(float));
    return float_as_float;
}

void bytestream_inject_f(struct bytestream * bs, float f)
{
    uint32_t float_as_u32 = 0u;
    memcpy(&float_as_u32, &f, sizeof(uint32_t));
    bytestream_inject_u32(bs, float_as_u32);
}

void bytestream_inject_string(struct bytestream * bs, const char * str)
{
    // + 1 is for the null character
    const size_t len
        = utilities_strlen(str, bs ? bs->capacity - bs->size - 1 : 0) + 1;
    verify_args(bs, len);
    if(!str)
    {
        error(FILE_LINE, "NULL ptr");
    }
    memcpy(&bs->data[bs->size], str, len);
    bs->size += len;
}

void bytestream_extract_string(struct bytestream * bs, char str[], size_t len)
{
    // verify with 0 length
    verify_args(bs, 0);
    if(!str)
    {
        error(FILE_LINE, "NULL ptr");
    }
    size_t in_len = utilities_strlen((char*)&bs->data[bs->size], bs->capacity);
    if(in_len >= bs->capacity)
    {
        error(FILE_LINE, "string not null terminated");
    }

    if(in_len >= len)
    {
        // the string will not fit in str
        error(FILE_LINE, "string too long");
    }

    memcpy(str, &bs->data[bs->size], in_len);
    str[in_len] = '\0';
    bs->size += in_len + 1; // plus one for null char
}

void bytestream_inject_u8_array(struct bytestream * bs,
                                const uint8_t bytes[],
                                size_t len)
{
    if(!bs || !bytes)
    {
        error(FILE_LINE, "NULL ptr");
    }
    if(bs->capacity - bs->size <= len)
    {
        error(FILE_LINE, "buffer overrun");
    }
    memcpy(&bs->data[bs->size], bytes, len);
    bs->size += len;
}
