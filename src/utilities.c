#include "nuhal/utilities.h"
#include "nuhal/error.h"
#include <string.h>

size_t utilities_strlen(const char str[], size_t maxlen)
{
    size_t index = 0;
    while(str[index] != '\0' && index < maxlen)
    {
        ++index;
    }
    return index;
}

