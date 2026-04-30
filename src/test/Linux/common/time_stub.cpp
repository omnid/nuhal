/// \file
/// \brief stub functions for time that are not implemented in a platform-independent way.  None of the platform-dependent behavior should be tested so all these functions throw exceptions
#include<stdexcept>
#include"nuhal/utilities.h"
#include"nuhal/time.h"


uint32_t time_current_ms(void)
{
    throw std::logic_error(FILE_LINE": STUB");
}

uint32_t time_current_us(void)
{
    throw std::logic_error(FILE_LINE": STUB");
}

uint32_t time_period_ms(void)
{
    throw std::logic_error(FILE_LINE": STUB");
}

uint32_t time_period_us(void)
{
    throw std::logic_error(FILE_LINE": STUB");
}
