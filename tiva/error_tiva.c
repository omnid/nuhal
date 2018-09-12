#include "common/error.h"
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include "driverlib/interrupt.h"

// tivaware assertion errors
void __error__(char * filename, uint32_t line)
{
    char linebuf[50] = "";
    snprintf(linebuf, sizeof(linebuf), ":%"PRIu32" TIVAWARE ASSERT", line);
    error(filename, linebuf);
}

__attribute__((weak))
void error_handler(__attribute__((unused)) const char * fileline,
                   __attribute__((unused)) const char * msg,
                   __attribute__((unused)) const void * data)
{
    // we don't want any interrupts continuing because otherwise
    // we could re-enter the error function repeatedly
    IntMasterDisable();

    for(;;)
    {
    }
}

__attribute__((weak))
void error_handler_fatal(__attribute__((unused)) const char * fileline,
                         __attribute__((unused)) const char * msg)
{
    for(;;)
    {
    }
}
