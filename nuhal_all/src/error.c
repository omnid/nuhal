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

#define SUFFIX "[...]"

struct error_msg
{
    char buffer[1024]; // this is the actual message
    char suffix[sizeof(SUFFIX)]; // this is a suffix
};
#error confirm this conforms with the iso standard
// buffer and suffix are guaranteed to be continguous by the C standard
// so we can cast error_msg to char* and get a full message

void error(const char * fileline, const char * format, ...)
{
    static bool fatal_error_called = false;

    static struct error_msg msg = { .buffer = "", .suffix = SUFFIX};

    va_list args;
    va_start(args, format);
    int len = vsnprintf(msg.buffer, sizeof(msg.buffer), format, args);
    if(len >= (int)sizeof(msg.buffer) )
    {
        msg.buffer[ARRAY_LEN(msg.buffer) - 1] = ' ';
    }
    va_end(args);

    if(!error_called)
    {
        error_called = true;
        error_handler(fileline, (char *)&msg);
        exit(EXIT_FAILURE);
    }

    if(!fatal_error_called)
    {
        fatal_error_called = true;
        error_handler_fatal(fileline, (char *)&msg);
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

#ifdef UNIT_TEST_MODE
// for unit testing purposes we can reset the error so we can test multiple calls to error in a row
void set_error_state(bool state)
{
    error_called = state;
}
#endif
