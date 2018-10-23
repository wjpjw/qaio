#pragma once
#include <stdint.h>

namespace wjp{

// event* is stored in epoll_event.data.ptr
// 3 types of events: io_event, timer_event, synchronizer_event
class event{
public:
    virtual int fd()const noexcept=0; // file descriptor is the common interface for sockets, normal file,  
    virtual void cb(uint32_t event_flag)=0; // called on epoll events
    virtual ~event(){}
};

}