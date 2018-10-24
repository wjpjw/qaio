#pragma once

#include "qaio.h"
#include <functional>
#include "thread.h"

namespace wjp{

class io_event : virtual public event{   
    using cb_func=std::function<void(uint32_t)>;
public:    
    io_event(cb_func cb, int fd, bool fast=false);
`   virtual ~io_event();
    void                    term();
    void                    rearm(int events);
    virtual int             fd()const noexcept{return fd_;}
    virtual void            cb(uint32_t event_flag);
private:    
    mutex                   mtx_;
    convar                  cv_;
    int                     fd_;
    bool                    fast_; 
    cb_func                 cb_;
    bool                    close_;
    int                     events_mask_; 
};

}

