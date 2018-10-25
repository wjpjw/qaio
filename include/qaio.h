#pragma once
#include <errno.h>          // errno
#include <sys/types.h>      // off_t, rlim_t
#include <sys/stat.h>
#include <sys/socket.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>         // strerror
#include <string>
#include <vector>
#include <stdexcept>
#include <limits>           // numeric_limits
#include <atomic>
#include <algorithm>
#include <functional>
#include <memory>

#if defined __GNUC__
#define likely(x) __builtin_expect ((x), 1)
#define unlikely(x) __builtin_expect ((x), 0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

namespace wjp
{
using str=const std::string&; // abbr. 
/*
Why not c-style callback?
-- c style function pointer plus void* args looks dangerous and ugly

Why is std::function<void()> the only prototype?
-- There are too many function prototypes; it's hard to define std::function for all of them. And it's even harder to come up with a name for them. 

How do you pass args?
-- I think lambda capture can replace args in majority of use cases; this lib is designed exlusively for modern c++ users.
*/
using func=std::function<void()>; // single prototype for all callbacks

}

#define QAIO
#include "error.h"

