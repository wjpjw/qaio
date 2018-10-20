#include "error.h"
#include <stdlib.h>

//Calling this function implies unacceptable flaws in program design. For example, failure of pthread_join() would trigger qpanic as it implies there exists deadlocks.   
void wjp::qpanic_if(bool fatal_error, const std::string& err_msg){
    if(unlikely(fatal_error)){
        /* 
            log error info and abort the program        
        */
       //@todo log
       abort();
    }
}

void wjp::qpanic(const std::string& err_msg){
    //log
    abort();
}

std::string wjp::qerror_str(str what, uint16_t level)
{
    static std::string errlevels[3]={"whoops | ", "panic | ", "crash | "};
    return errlevels[level] + what + std::string(" | strerror: ")+strerror(errno);
}