#ifndef QAIO
#include "qaio.h"
#endif

namespace wjp{


extern std::string qerror_str(str what, uint16_t level);

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

//If is_error is very unlikely(less than say 5% chance) to be true, this function could potentially enhance branch prediction.
static inline void qerror_if(bool is_error, const std::string& err_msg){
    if(unlikely(is_error)) throw qerror(err_msg);
}

//Calling this function implies unacceptable flaws in program design. For example, failure of pthread_join() would trigger qpanic as it implies there exists deadlocks.   
extern void qpanic_if(bool fatal_error, const std::string& err_msg);

extern void qpanic(const std::string& err_msg);
}
