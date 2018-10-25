#pragma once

#include "qaio.h"
#include <functional>
#include "posix/network/epoller.h"
#include "posix/network/efd.h"
#include "thread.h"
#include "threadpool.h"
namespace wjp{

class eventloop{
protected:
    constexpr static uint32_t enabled=EPOLLIN|EPOLLOUT|EPOLLERR;
    // instance creation should be inaccessible to users
    eventloop(int nr_threads): tp_(nr_threads){}
public: 
    // single global instance
    static eventloop&       me() 
    {
        static eventloop instance_;
        return instance_;
    }
    threadpool&             tp() 
    {
        return tp_;
    }
    void                    loop();
    void                    efd_clear();
    void                    efd_notify();
    void                    add_event(int fd);
    void                    del_event(int fd);
    void                    rearm_event(event* ev, int events_old, int events_append);
private:
    epoller                 epoller_; //thread-safe
    bool                    shutdown_ = false; // used in loop()
    mutex                   mtx_;
    threadpool              tp_;
    //timerfd-based timer; eventfd-based synchronizer
};

}