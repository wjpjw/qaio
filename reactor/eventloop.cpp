#include "eventloop.h"

namespace wjp{

// run in current thread (usually main thread) & block; 
void                    eventloop::loop()
{        	
    while(!shutdown_){
        int n=epoller_.wait();  //call epoll_wait
        for(int i=0; i<n; i++){
            auto epoll_event_ptr=epoller_.events()+i;
            // ev points to an event object
            event* ev     = epoll_event_ptr->data.ptr;
            uint32_t flag = epoll_event_ptr->events|enabled;
            ev->cb(flag);
        }
    }
}

void                    eventloop::efd_clear()
{
    efd_recv(event_fd_);
}

void                    eventloop::efd_notify()
{
    efd_send(event_fd_);
}

void                    eventloop::add_event(int fd){
    epoller_.add(fd, this);
}

void                    eventloop::del_event(int fd){
    epoller_.del(fd);
}

void                    eventloop::rearm_event(event* ev, int events_old, int events_append){
    epoller_.mod(ev->fd(), events_old, events_append, ev);
}



}   

