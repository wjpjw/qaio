#include "io_event.h"
#include "reactor_threadpool.h"

namespace wjp{

io_event::io_event(std::function<void(uint32_t)> cb, int fd, bool fast=false) : cb_(cb) , close_(false), events_mask_(0), fd_(fd), cv_(mtx_), fast_(fast)
{
    eventloop::me().add_event(fd);
}

io_event::~io_event()
{
    term();
    close(fd_); 
}

void                    io_event::term()
{
    lock lk(mtx_);
    if(!close_){
        close_=true;
        shutdown(fd_, SHUT_RDWR); 
        eventloop::me().del_event(fd_);
    }
}
void                    io_event::rearm(int events)
{
    lock lk(mtx_);
    if(!close_){
        eventloop::me().rearm_event(fd_, events_mask_, events, this);
    }
}

int                     io_event::fd()const noexcept{return fd_;}

void                    io_event::cb(uint32_t event_flag)
{
    // cb can be called without mtx_ protection
    if(fast_){
        // fast_==true indicates that this cb can finish boundedly therefore it does not need to be pushed into threadpool
        cb_(event_flag);
    }else{
        // threadpool accepts function<void()> type only; there is a way to work around it
        eventloop::tp().execute([event_flag]{
            cb_(event_flag);
        });
    }
}


}