#include "common/error.h"
#include <stdio.h>
#include <stdlib.h>

__attribute__((weak))
void error_handler(const char * fileline,
                   const char * msg)
{
    fprintf(stderr, "HOST ERROR: %s %s\n", fileline, msg);
}

__attribute__((weak))
void error_handler_fatal(const char * fileline,
                         const char * msg)
{
    fprintf(stderr, "FATAL HOST ERROR: %s %s\n", fileline, msg);
}
