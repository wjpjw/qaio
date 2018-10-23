#include "epoller.h"
#include <fcntl.h>          // fcntl, posix_fadvise
#include <sys/stat.h>       // stat, S_ISREG
#include <unistd.h>         // read, open, write, close, lseek, pread
#include <sys/epoll.h>

namespace wjp{

epoller::epoller(){
    epoll_fd_=epoll_create1(EPOLL_CLOEXEC);
    qerror_if(epoll_fd_<0, "Fail to epoll_create1()");
}

epoller::~epoller()
{
    close(epoll_fd_);
}

void                    epoller::add(int fd, void* ev_data){
    fcntl(fd, F_SETFD, FD_CLOEXEC);
    fcntl(fd, F_SETFL, O_NONBLOCK);
    struct epoll_event ee;
    ee.events=0;
    ee.data.ptr=ev_data;
    qerror_if ((epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ee)) != 0, "Fail to add epoll event");
}

void                    epoller::mod(int fd, int events_new, void* ev_data)
{
    struct epoll_event ee;
    ee.events   = events_new | EPOLLONESHOT | EPOLLERR;
    ee.data.ptr = ev_data; 
    qerror_if(epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ee) != 0, "Fail to mod epoll event"); 
}

void                    epoller::mod(int fd, int events_old, int events_append, void* ev_data)
{
    mod(fd, events_old | events_append, ev_data);
}

void                    epoller::del(int fd){
    struct epoll_event ee; //unused
    qerror_if(epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &ee)!=0, "Fail to del epoll event");
}

int                     epoller::wait()
{
    int n = epoll_wait(epoll_fd_, events_, max_nr_epoll_events, -1);
    qerror_if(n<0, "epoll_wait failed");
    return n; 
}


    
}