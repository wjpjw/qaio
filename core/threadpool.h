#pragma once
#include "qaio.h"

namespace wjp {

/*
    threadpool is a fixed-sized RAII threadpool.
    It has no control over individual thread.
*/
struct threadpool {
public:
    explicit threadpool(int nr_thread); // init with number of threads 
    threadpool() = delete;
    threadpool(threadpool &&) = default;
    ~threadpool(); // shutdown could potentially block since we use join by default
    void execute(func &&task); // kicks off a task; wake up one thread
private:
    mutex mtx_;
    convar cond_;
    bool is_shutdown_;  
    bool started_;
    std::queue<func> tasks_;  //@toimprove stl queue + mutex is sub-ideal 
};

}

