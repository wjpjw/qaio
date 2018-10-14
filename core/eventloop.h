#pragma once

#include "qaio.h"
#include <mutex>
#include <functional>
#include <condition_variable>
#include <thread>
#include "posix/network/epoller.h"
#include "posix/network/efd.h"
#include "thr.h"
#include "convar.h"

namespace wjp{

class eventloop{
public: 
    // foo is used to denote single global instance in this project
    static eventloop&       foo() 
    {
        static eventloop instance_;
        return instance_;
    }

    // allow user created instances in rare meaningful cases
    eventloop(){}

    // run in current thread (& block)
    void                    run()
    {

    }

    // run in background thread
    void                    kick_it_off()
    {

    }
    void                    efd_clear()
    {
        efd_recv(event_fd_);
    }
    void                    efd_notify()
    {
        efd_send(event_fd_);
    }

    void                    add_event(int fd){
        epoller_.add(fd);
    }

    void                    del_event(int fd){
        epoller_.del(fd);
    }

    void                    rearm_event(int fd, int events_old, int events_append, void* ev_data){
        epoller_.mod(fd, events_old, events_append, ev_data);
    }
    
    thr                     backgroud_thread(){
        return thread_;
    }
    //@todo: use lockfree or fine-grained concurrent list
    void                    add_to_be_removed(io_event* io_event_)
    {
        std::lock_guard<std::mutex> lk(mtx_);
        to_be_removed_.push_back(io_event_);
    }
private:
    thr                     thread_;
    epoller                 epoller_; //thread-safe
    bool                    shutdown_;
    std::mutex              mtx_;
    int                     timer_fd_;
    int                     event_fd_;
    std::vector<io_event*>  to_be_removed_; //pending io_events  
};


// 通常io_event应该由用户代码持有
class io_event{
public:    
    io_event(){} // stl compat
    io_event(std::function<void()> cb, int fd) : cb_(cb) , closed_(false), closing_(false), reap_(false), events_mask_(0), fd_(fd)
    {
        eventloop::foo().add_event(fd);
    }

    // 如果该对象由eventloop背景线程之外的线程持有并释放，则必须通过eventfd通知eventloop，让它移除这个io_event。这里会等待直到eventloop的移除完毕，因此会阻塞短暂的时间。
    ~io_event()
    {
        term();
        if(!eventloop::foo().backgroud_thread().is_current_thread()){
            eventloop::foo().add_to_be_removed(this);
            eventloop::foo().efd_notify();

        }

    }

    void                    term()
    {
        std::lock_guard<std::mutex> lk(mtx_);
        if(!closing_){
            closing_=true;
            shutdown(fd_, SHUT_RDWR); 
            eventloop::foo().del_event(fd_);
        }
    }
    void                    rearm(int events)
    {
        std::lock_guard<std::mutex> lk(mtx_);
        if(!closing_){
            eventloop::foo().rearm_event(fd_, events_mask_, events, this);
        }
    }

private:    
    std::mutex              mtx_;
    int                     fd_;
    std::function<void()>   cb_;
    bool                    closed_;
    bool                    closing_;
    bool                    reap_;
    int                     events_mask_; 
    convar                  cv_;
};


}