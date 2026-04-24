#ifndef NUHAL_ERROR_H_INCLUDE_GUARD
#define NUHAL_ERROR_H_INCLUDE_GUARD

/// @brief platform-independent error handling code
/// @file
///  The error handling behavior for this project is to
///  Notify the user and exit the program. On a microcontroller
///  we may loop infinitely to preserve state instead of exiting

#include "nuhal/utilities.h"
#include <stdbool.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Trigger an error condition.  This will call the
/// platform/application-specific error_handler.  It also prevents
/// recursive calls to error.  If a function used in the error handler
/// itself calls error, than the error function terminates the program
/// @param fileline: the FILE_LINE macro, current file and line number
/// @param format: formatted C-style string
/// @param ...: variable number of arguments to be placed into format
/// @post whether the program loops forever or exits is platform dependent
void error(const char * fileline, const char * format, ...) __attribute__((noreturn, format(printf, 2, 3)));

/// @brief a platform-dependent function that handles the error message
/// @param fileline - filename and line-number where the error was triggered
/// @param msg - custom additional message
/// @details
/// The platform may provide a generic weak version of the error_handler
/// which can be overridden by specific applications
void error_handler(const char * fileline, const char * msg);

/// @brief a platform-dependent function that handles when an error occurs 
/// within the error handler.  This function SHOULD NOT call any function
/// that might call error
void error_handler_fatal(const char * fileline, const char * msg);

/// @brief display an error message corresponding to the errno that is set
///  and exit the program
/// @param fileline: the FILE_LINE macro, current file and line number
void error_with_errno(const char * fileline) __attribute__((noreturn));

/// @brief return true if an error is already pending, false otherwise
/// This function is useful in code that may run from within the error handler
/// as it allows such code to avoid triggering a recursive error
bool error_pending(void);

#ifdef UNIT_TEST_MODE
    // This function is for UNIT TESTING ONLY and should not be present in production code!
    // reset the error so we can test multiple calls to error in a row
    // state - if false, no error is pending, if true it is as if error() has been called once
    void set_error_state(bool state);
#endif

#ifdef __cplusplus
}
#endif

#endif
