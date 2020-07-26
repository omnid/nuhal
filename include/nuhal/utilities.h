#ifndef COMMON_UTILITIES_H
#define COMMON_UTILITIES_H
/// @file
/// @brief commonly useful c utilities
#include<stdint.h>
#include <stddef.h>

// helpers for FILE_LINE macro
#define FILE_LINE_STRINGIFY(file,line) file":"#line
#define FILE_LINE_EXPAND(file,line) FILE_LINE_STRINGIFY(file,line)

/// @brief String literal in the form of the current_file:current_line
#define FILE_LINE FILE_LINE_EXPAND(__FILE__, __LINE__)


/// @brief determine the length of a statically allocated array
#define ARRAY_LEN(x) (sizeof((x))/sizeof((x)[0]))

/// @brief cause a compile error if test fails
/// see https://stackoverflow.com/questions/3385515/static-assert-in-c
/// test - a boolean test that can be evaluated at compile time
/// msg - a unique message that is also a valid c identifier
#define STATIC_ASSERT(test, msg) \
    __attribute__((unused)) typedef char static_assert_##msg[!!(test) - !(test)]

/// @brief pi
#define PI 3.14159265358979323846f

/// @brief acceleration due to gravity m/s
#define GRAVITY_ACCEL 9.8f

/// @brief convert radians to degrees
#define RADIANS_TO_DEGREES(rad) ((rad) * 180.0f/PI)

#define DEGREES_TO_RADIANS(deg) ((deg) * PI/180.0f)

/// @brief convert radians/sec to revolutions/sec
#define RADSEC_TO_REVSEC(radsec) ((radsec) * 2.0f * PI)

/// @brief convert rpm to rad/sec
#define RPM_TO_RADSEC(rpm) (((rpm) * 2.0f * PI)/60.0f)
#ifdef __cplusplus
extern "C" {
#endif

/// @brief get the length of a null-terminated string with max length maxlen
/// @param str - the string to check the length of
/// @param maxlen - the max length of the string including the null character
/// @return the length of the string, excluding the null character.
/// or maxlen if the maximum length was reached
size_t utilities_strlen(const char str[], size_t maxlen);

#ifdef __cplusplus
}
#endif

#endif
