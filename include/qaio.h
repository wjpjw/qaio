#pragma once
#include <errno.h>          // errno
#include <sys/types.h>      // off_t, rlim_t
#include <stdint.h>
#include <stdlib.h>
#include <string.h>         // strerror
#include <string>
#include <vector>
#include <stdexcept>
#include <limits>           // numeric_limits
#include <atomic>
#include <algorithm>

#define  likely(x)        __builtin_expect(!!(x), 1) 
#define  unlikely(x)      __builtin_expect(!!(x), 0) 

namespace wjp
{
using str=const std::string&; // abbr.

static inline std::string               qerror_str(str what, uint16_t level)
{
    static std::string errlevels[3]={"whoops | ", "panic | ", "crash | "};
    return errlevels[level] + what + std::string(" | strerror: ")+strerror(errno);
}

class qerror : public std::runtime_error
{
public:
    enum error_level : uint16_t{
        WHOOPS=0, // resource unavailable or input illegal; properly handled (or ignored)
        PANIC=1, // unknown error caught; report it for debugging purpose
        CRASH=2 // fatal error detected; logical to abort
    };
    qerror(str what, error_level level=WHOOPS) 
        : runtime_error(qerror_str(what, level)), error_level_(level){}
    error_level error_level_;    
};



}