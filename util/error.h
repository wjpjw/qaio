#pragma once
#include <stdexcept>
#include <stdint.h>
#include "qaio.h"
namespace wjp
{

static inline std::string               quasi_error_str(str what, uint16_t level)
{
    static std::string errlevels[3]={"whoops | ", "panic | ", "crash | "};
    return errlevels[level] + what + std::string(" | strerror: ")+strerror(errno);
}

class quasi_error : public std::runtime_error
{
public:
    enum error_level : uint16_t{
        WHOOPS=0, // resource unavailable or input illegal; properly handled (or ignored)
        PANIC=1, // unknown error caught; report it for debugging purpose
        CRASH=2 // fatal error detected; logical to abort
    };
    quasi_error(str what, error_level level=WHOOPS) 
        : runtime_error(quasi_error_str(what, level)), error_level_(level){}
    error_level error_level_;    
};

}