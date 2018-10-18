#pragma once

#include "qaio.h"

namespace wjp{

/*

The event argument describes the object linked to the file descriptor fd. The struct epoll_event is defined as :

typedef union epoll_data {
    void        *ptr;
    int          fd;
    uint32_t     u32;
    uint64_t     u64;
} epoll_data_t;

struct epoll_event {
    uint32_t     events;     // Epoll events
    epoll_data_t data;       // User data variable
};

The events member is a bit set composed using the following available event types:
EPOLLIN
    The associated file is available for read(2) operations.
EPOLLOUT
    The associated file is available for write(2) operations.
EPOLLRDHUP (since Linux 2.6.17)
    Stream socket peer closed connection, or shut down writing half of connection. (This flag is especially useful for writing simple code to detect peer shutdown when using Edge Triggered monitoring.)
EPOLLPRI
    There is urgent data available for read(2) operations.
EPOLLERR
    Error condition happened on the associated file descriptor. epoll_wait(2) will always wait for this event; it is not necessary to set it in events.
EPOLLHUP
    Hang up happened on the associated file descriptor. epoll_wait(2) will always wait for this event; it is not necessary to set it in events.
EPOLLET
    Sets the Edge Triggered behavior for the associated file descriptor. The default behavior for epoll is Level Triggered. See epoll(7) for more detailed information about Edge and Level Triggered event distribution architectures.
EPOLLONESHOT (since Linux 2.6.2)
    Sets the one-shot behavior for the associated file descriptor. This means that after an event is pulled out with epoll_wait(2) the associated file descriptor is internally disabled and no other events will be reported by the epoll interface. The user must call epoll_ctl() with EPOLL_CTL_MOD to rearm the file descriptor with a new event mask.

wjp::epoller uses:
1. default level-triggered mode: it makes epoll simply a faster poll
2. EPOLLONESHOT: Since even with edge-triggered epoll, multiple events can be generated upon receipt of multiple chunks of data, the caller has the option to specify the EPOLLONESHOT flag, to tell epoll to disable theassociated file descriptor after the receipt of an event with epoll_wait(2).  When the EPOLLONESHOT flag is specified, it is the caller's responsibility to rearm the file descriptor using epoll_ctl(2) with EPOLL_CTL_MOD. Since we use a set of threads to handle epoll events, one-shot mode is desirable.
3. EPOLLERR: enables error detection 

Edge-triggered mode: edge-triggered mode delivers events only when changes occur on the monitored file descriptor. 

ET epoll generate more than one event for the same receipt of data, if a buffer overflow happens.

ET mode still seems broken. See:
    https://stackoverflow.com/questions/12892286/epoll-with-edge-triggered-event/12897334

```
... the exact behaviour of edge-triggered mode actually depends on the socket type used and isn't really documented anywhere. for datagram sockets you might get more events than you would expect.
... and for eventfds and timerfds you get fewer than you'd expect.
```

*/

//@thread-safe
class epoller{
public:
    epoller();
    ~epoller();
    void                    add(int fd);
    void                    mod(int fd, int events_new, void* ev_data);
    void                    mod(int fd, int events_old, int events_append, void* ev_data);
    void                    del(int fd);
    int                     fd() const noexcept {return epoll_fd_;}
private:
    int                     epoll_fd_;
};



}