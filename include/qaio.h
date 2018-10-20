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

}

#define QAIO
#include "error.h"

