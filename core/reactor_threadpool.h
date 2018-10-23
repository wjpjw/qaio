#pragma once

#include "qaio.h"

namespace wjp{

// global single instance
// responsible for all reactor event handling routines
class reactor_threadpool{
public:
    static reactor_threadpool&       me() 
    {
        static reactor_threadpool instance_;
        return instance_;
    }
    void execute(func task_func){
        
    }

private:


};


}