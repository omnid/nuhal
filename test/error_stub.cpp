/// @file
/// @brief Override error handlers and make them throw C++ exceptions.
///   This makes the C++ testing easier when something happens
#include<stdexcept>
#include"nutilities/error.h"


void error_handler(const char * fileline, const char * msg)
{
    const std::string fline(fileline);
    const std::string smsg(msg);
    throw std::logic_error(fline+": " + smsg);
}

void error_handler_fatal(const char * fileline, const char * msg)
{
    const std::string fline(fileline);
    const std::string smsg(msg);
    throw std::logic_error(fline+": (fatal) " + smsg);
}
