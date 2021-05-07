#include "nuhal/utilities.h"
#include "nuhal/error.h"
#include <string.h>
#include <math.h>

size_t utilities_strlen(const char str[], size_t maxlen)
{
    size_t index = 0;
    while(str[index] != '\0' && index < maxlen)
    {
        ++index;
    }
    return index;
}

float normalize_angle(float rad)
{
    const float a = fmodf(rad, 2.0f * PI);
    if(a > PI)
    {
        return a - 2.0f *PI;
    }
    else if(a < -PI)
    {
        return a + 2.0f * PI;
    }
    return a;
}