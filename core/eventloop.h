#pragma once

#include "qaio.h"
#include <mutex>
#include <unistd.h>
#include <functional>
#include <condition_variable>

namespace wjp{

class iofd{
public:    
    iofd(std::function<void()> cb, int epoll_fd) : cb_(cb) , closed_(false), closing_(false), reap_(false), events_flag_(0)
    {
        fcntl(fd, F_SETFD, FD_CLOEXEC);
        fcntl(fd, F_SETFL, O_NONBLOCK);
        struct epoll_event ee;
        ee.events=0;
        ee.data.ptr=this;
        if ((epoll_ctl(epoll_fd,EPOLL_CTL_ADD, fd, &ee)) != 0) {
            throw qerror("Fail to add epoll event");
	    }
    }
private:    
    std::mutex              mtx_;
    int                     fd_;
    std::function<void()>   cb_;
    bool                    closed_;
    bool                    closing_;
    bool                    reap_;
    int                     events_flag_; 
    std::condition_variable cv_;
};

class eventloop{
public: 
    eventloop() 
    {
        std::thread([](){

        }).detach();
    }
private:
    bool                    shutdown_;
    std::mutex              mtx_;
    int                     epoll_fd_;
    int                     timer_fd_;
    int                     event_fd_;
};

}