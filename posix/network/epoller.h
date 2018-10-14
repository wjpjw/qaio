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


wjp::epoller使用了下列设置：
 1. 默认的level-triggered模式
 2. ONESHOT模式
 3. 开启EPOLLERR错误事件
 
 ONESHOT与ET的区别：
 ET在缓冲区溢出时就会产生通知，如果缓冲区足够大，它的确可以保证同一种事件仅通知一次，可读可写错误三类事件独立。这看起来很美好，但如果缓冲区不够大就会导致同一事件通知多次。在线程池工作队列的设计中，这就导致一个socket可能被多个线程own。

 ONESHOT让对某fd的监听行为变成一次性的，一个事件永远只有一次通知，而且通知完了还得rearm这个fd，保证一个socket总是被一个线程own。

不用epoll的ET模式主要还是因为它的行为会依赖于不同的socket，有不确定性，详见这个答案：
    https://stackoverflow.com/questions/12892286/epoll-with-edge-triggered-event/12897334

对timerfd, eventfd会通知得比预期的少；datagram sockets则通知得比预期的多；
略坑而且完全没有注释，所以实际工程里没理由用它。

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