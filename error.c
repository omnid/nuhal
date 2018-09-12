#include "common/error.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

static const void * error_data = NULL;

void error_setup(const void * data)
{
    error_data = data;
}

void error_with_errno(const char * fileline)
{
    error(fileline, strerror(errno));
}

void error(const char * fileline, const char * msg)
{
    // sometimes an error somewhere else can trigger another
    // error message. we want to stop recursive error calls
    static bool error_called = false;
    static bool fatal_error_called = false;
    if(!error_called)
    {
        error_called = true;
        error_handler(fileline, msg, error_data);
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

