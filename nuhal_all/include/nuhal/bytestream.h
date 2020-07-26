#ifndef COMMON_ALL_BYTESTREAM_H
#define COMMON_ALL_BYTESTREAM_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/// @brief convert between streams of bytes and other types
/// This file provides the basic framework and built-in types
/// the user should add their own extraction and injection 
/// functions for custom types.

/// @brief used to deserialize data from a byte stream
/// this struct tracks the position in the stream
struct bytestream
{
    // the actual data
    uint8_t * data; 
    // the length in bytes of the data buffer
    size_t capacity;

    // the current size of the data that has been processed
    // this is the position in data where the next byte
    // should be read to/written from
    size_t size;
};


#ifdef __cplusplus
extern "C" {
#endif

/// @brief initialize a byte stream, or reset it to its original position
/// @param out - the bytestream to initialize
/// @param data - the buffer holding the bytes
/// @param len - the length of the buffer
void bytestream_init(struct bytestream * out, uint8_t * data, size_t len);

/// @brief place a single byte in the bytestream
/// @param bs - the bytestream to modify
/// @param u8 - the byte to place into the stream
void bytestream_inject_u8(struct bytestream * bs, uint8_t u8);

/// @brief get the next byte in the bytestream
/// @param bs - the bytestream
/// @return the next byte
uint8_t bytestream_extract_u8(struct bytestream * bs);

/// @brief convert the next data in the stream to a u16
/// @param bs - the bytestream, which will be advanced by two bytes
/// @return the uint16 in the bytestream
uint16_t bytestream_extract_u16(struct bytestream * bs);

/// @brief place a uint16_t into the bytestream
/// @param bs - the bytestream, which will be advanced by two bytes
/// @param u16 - the data to place in the stream, in little endian format
void bytestream_inject_u16(struct bytestream * bs, uint16_t u16);

/// @brief get uint32_t from the bytestream
/// @param bs - the bytestream, which will be advanced by two bytes
/// @return the next uint32 from the stream
uint32_t bytestream_extract_u32(struct bytestream * bs);

/// @brief place a uint16_t into the bytestream
/// @param bs - the bytestream, which will be advanced by two bytes
/// @param u32 - the data to place in the stream, in little endian format
void bytestream_inject_u32(struct bytestream * bs, uint32_t u32);

/// @brief get int32_t from the bytestream
/// @param bs - the bytestream, which will be advanced by two bytes
/// @return the next int32 from the stream
int32_t bytestream_extract_i32(struct bytestream * bs);

/// @brief place a uint16_t into the bytestream
/// @param bs - the bytestream, which will be advanced by two bytes
/// @param i32 - the data to place in the stream, in little endian format
void bytestream_inject_i32(struct bytestream * bs, int32_t i32);


/// @brief read a float from a bytestream
/// @param bs - the bytestream, which will be advanced by two bytes
/// @return the next data, as a float from the bytestream
float bytestream_extract_f(struct bytestream * bs);

/// @brief place a float into the bytestream
/// @param bs - the bytestream, which will be advanced by two bytes
/// @param f - the data to place in the stream, in little endian.
void bytestream_inject_f(struct bytestream * bs, float f);

/// @brief place a c style string into the bytestream
/// @param bs - the bytestream, which will be advanced by two bytes
/// @param s - null terminated string to put into the stream. the null character
/// is included
void bytestream_inject_string(struct bytestream * bs, const char * str);

/// @brief extract a string from the bytestream.
/// @param bs - the bytestream which should contain ascii characters
/// followed by a null character to store in str. if the end of the stream
/// is reached without a null character an error will be issued
/// @param str - where the string is to be stored.
/// @param len - the max length of the output string including '\0'.
///              no more than len-1 characters will be read from the bytestream.  
/// @post it is an error if full string in the bytestream does not fit in str
void bytestream_extract_string(struct bytestream * bs, char str[], size_t len);

/// @brief inject an array of bytes into a bytestream
/// @param bs - the bytestream, which should have enough room to store array
/// @param bytes - the bytes to inject into the stream
/// @param len - the length of the bytes to writek
void bytestream_inject_u8_array(struct bytestream * bs,
                                const uint8_t bytes[],
                                size_t len);

#ifdef __cplusplus
}
#endif

#endif
