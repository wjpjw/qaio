#pragma once

#include "qaio.h"
#include <functional>
#include "thread.h"

namespace wjp{

class io_event : virtual public event{   
    using cb_func=std::function<void(uint32_t)>;
public:    
    io_event(int fd, cb_func cb=nullptr);
`   virtual ~io_event();
    virtual void            term();
    virtual void            rearm(int events);
    virtual void            set_cb(cb_func cb);
    virtual int             fd()const noexcept{return fd_;}
    virtual void            cb(uint32_t event_flag);
private:    
    mutex                   mtx_; // protect close_
    int                     fd_;
    cb_func                 cb_;
    bool                    close_          = false;
    int                     events_mask_    = 0; 
};

}

