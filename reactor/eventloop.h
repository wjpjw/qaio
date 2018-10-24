#pragma once

#include "qaio.h"
#include <functional>
#include "posix/network/epoller.h"
#include "posix/network/efd.h"
#include "thread.h"

namespace wjp{

class eventloop{
protected:
    constexpr static uint32_t enabled=EPOLLIN|EPOLLOUT|EPOLLERR;
    // instance creation should be inaccessible to users
    eventloop(): thread_(), shutdown_(false){}
public: 
    // single global instance in this project
    static eventloop&       me() 
    {
        static eventloop instance_;
        return instance_;
    }
    // run in current thread (usually main thread) & block; 
    void                    loop();
    // run in a background thread
    void                    kick_it_off();
    void                    efd_clear();
    void                    efd_notify();
    void                    add_event(int fd);
    void                    del_event(int fd);
    void                    rearm_event(event* ev, int events_old, int events_append);
private:
    thread                  thread_; 
    epoller                 epoller_; //thread-safe
    bool                    shutdown_; // used in loop()
    mutex                   mtx_;
    //timerfd-based timer; eventfd-based synchronizer
};

}