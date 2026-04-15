#include "nuhal/error.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

void error_with_errno(const char * fileline)
{
    error(fileline, strerror(errno));
}

// sometimes an error somewhere else can trigger another
// error message. we want to stop recursive error calls
static bool error_called = false;

void error(const char * fileline, const char * format, ...)
{
    static bool fatal_error_called = false;

    if(!error_called)
    {
        error_called = true;
        va_list args;
        va_start(args, format);

        // determine required size because read length or write length might be greater than standard 1024 buffer size
        va_list args_copy;
        va_copy(args_copy, args);
        int needed = vsnprintf(NULL, 0, format, args_copy);
        va_end(args_copy);

        if (needed < 0) {
            va_end(args);
            error_handler(fileline, "Invalid formatted error message");
            exit(EXIT_FAILURE);
        }

        // allocate buffer (stack or heap)
        char *msg = malloc((size_t)needed + 1);
        if (!msg) {
            va_end(args);
            error_handler(fileline, "Unable to allocate memory for error message");
            exit(EXIT_FAILURE);
        }

        vsnprintf(msg, (size_t)needed + 1, format, args);
        va_end(args);
        error_handler(fileline, msg);
        free(msg);
        exit(EXIT_FAILURE);
    }

    if(!fatal_error_called)
    {
        fatal_error_called = true;
        va_list args;
        va_start(args, format);

        // determine required size because read length or write length might be greater than standard 1024 buffer size
        va_list args_copy;
        va_copy(args_copy, args);
        int needed = vsnprintf(NULL, 0, format, args_copy);
        va_end(args_copy);

        if (needed < 0) {
            va_end(args);
            error_handler_fatal(fileline, "Invalid formatted error message");
            exit(EXIT_FAILURE);
        }

        // allocate buffer (stack or heap)
        char *msg = malloc((size_t)needed + 1);
        if (!msg) {
            va_end(args);
            error_handler_fatal(fileline, "Unable to allocate memory for error message");
            exit(EXIT_FAILURE);
        }

        vsnprintf(msg, (size_t)needed + 1, format, args);
        va_end(args);
        error_handler_fatal(fileline, msg);
        free(msg);
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
