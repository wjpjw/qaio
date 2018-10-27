#include "io_event.h"
#include "eventloop.h"

namespace wjp{

io_event::io_event(int fd, cb_func cb) : cb_(cb) ,fd_(fd)
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
    mtx_.lock();
    if(!close_){
        close_=true;
        mtx_.unlock();
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

void                    io_event::set_cb(cb_func cb)
{
    cb_=cb;
}

int                     io_event::fd() const noexcept
{
    return fd_;
}

void                    io_event::cb(uint32_t event_flag)
{
    cb_(event_flag);
}


}