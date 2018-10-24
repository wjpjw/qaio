#pragma once

#include "qaio.h"
#include "event.h"

namespace wjp{

class sync_event  : virtual public event{
public:
    virtual int             fd()const noexcept{return fd_;}
    virtual void            cb(uint32_t event_flag);
private:
    int                     fd_;
};


}