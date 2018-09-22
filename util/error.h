#pragma once
#include <stdexcept>
#include <stdint.h>
#include "qaio.h"
namespace wjp
{

class quasi_error : public std::runtime_error
{
public:
    enum error_level : uint16_t{
        WHOOPS=0, // resource unavailable or input illegal; handled by try catch
        PANIC=1, // unknown error caught; report it for debugging purpose
        CRASH=2 // fatal error detected; logical to abort
    };
    explicit quasi_error(int errno, str what, error_level level=WHOOPS) 
        : runtime_error(what+std::string(" | strerror: ")+strerror(errno)), error_level_(level){}
    explicit quasi_error(str what, error_level level=WHOOPS) 
        : runtime_error(what), error_level_(level){}
    error_level error_level_;    
};

}