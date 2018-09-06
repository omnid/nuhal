#ifndef COMMON_ERROR_H_INCLUDE_GUARD
#define COMMON_ERROR_H_INCLUDE_GUARD

/// @brief platform-independent error handling code
///  The error handling behavior for this project is to
///  Notify the user and exit the program. On a microcontroller
///  we may loop infinitely to preserve state instead of exiting

#include "common/utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @brief initialize the error handling system
/// @param data - this pointer is passed to the error hook
/// function when an error is triggered. If this function
/// is never called, the hook will receive a NULL as its data
void error_setup(const void * data);

/// @brief Trigger an error condition.  This will call the
/// platform/application-specific error_handler.  It also prevents
/// recursive calls to error.  If a function used in the error handler
/// itself calls error, than the error function terminates the program
/// @param fileline: the FILE_LINE macro, current file and line number
/// @param msg: additional error information
/// @post whether the program loops forever or exits is platform dependent
void error(const char * fileline, const char * msg) __attribute__((noreturn));

/// @brief a platform-dependent function that handles the error message
/// @param fileline - filename and line-number where the error was triggered
/// @param msg - custom additional message
/// @param data - additional data, as provided to error_setup
/// The platform may provide a generic weak version of the error_handler
/// which can be overridden by specific applications
void error_handler(const char * fileline, const char * msg, const void * data); 

/// @brief display an error message corresponding to the errno that is set
///  and exit the program
/// @param fileline: the FILE_LINE macro, current file and line number
void error_with_errno(const char * fileline) __attribute__((noreturn));




#ifdef __cplusplus
}
#endif

#endif
