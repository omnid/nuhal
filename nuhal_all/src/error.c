#include "nuhal/error.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>


void error_with_errno(const char * fileline)
{
    error(fileline, "%s", strerror(errno));
}

// sometimes an error somewhere else can trigger another
// error message. we want to stop recursive error calls
static bool error_called = false;


void error(const char * fileline, const char * format, ...)
{
    static bool fatal_error_called = false;

    static char msg[] = { [ERROR_MAX_ERROR_LEN] = '[', '.', '.', '.', ']'};

    va_list args;
    va_start(args, format);
    int len = vsnprintf(msg, ERROR_MAX_ERROR_LEN, format, args);
    if(len >= ERROR_MAX_ERROR_LEN )
    {
        msg[ARRAY_LEN(msg) - 1] = ' ';
    }
    va_end(args);

    if(!error_called)
    {
        error_called = true;
        error_handler(fileline, msg);
        exit(EXIT_FAILURE);
    }

    if(!fatal_error_called)
    {
        fatal_error_called = true;
        error_handler_fatal(fileline, msg);
        exit(EXIT_FAILURE);
    }

    // if we get here both error_called and
    // fatal error_called are true
    exit(EXIT_FAILURE);
}


bool error_pending(void)
{
    return error_called;
}

// Internal, for unit testing purposes only.
// DO NOT CALL THIS UNLESS IN A UNIT TEST
void TEST_set_error_state(bool state)
{
    error_called = state;
}
