#pragma once

#include "qaio.h"
#include <mutex>

namespace wjp{


class poller{
public:

private:
    std::mutex              mtx_;
    int                     epoll_fd_;
    int                     timer_fd_;
    int                     event_fd_;

};

}