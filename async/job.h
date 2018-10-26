#pragma once
#include "qaio.h"

namespace wjp{


/*
    A job is a shared object owned and posted by a client.
    It can be dispatched to a background thread specialized in handling certain time-consuming routine like name resolving, reaping, etc.
    The client can do something else and then wait on the it after posting the job. It will be waked upon job completion.
*/
class job{
public:
    // Jobs are designed to be cancellable; clients need to provide a customed cancel function in order to cancel a job gracefully.
    // 
    job(func on_finish, func on_cancel):cv(mtx)
    {}
private:
    bool finish = false;    // the shared state of its completion
    bool ready = false;     // before ready
    time expire = 0;     // when to expire
    time duration = 0;  // enque time + duration = expire time
    bool close = false;     // the state of being torn down
    bool sleep = false;     // the state of sleeping (for a while) 
    func on_finish; // called after provider completes its job; set finish=true when this callback returns 
    func on_cancel; // called on cancel_job() or job expiration(if timeout is not infinite); it should tear down the job and roll back its side-effects
    convar cv;
    mutex mtx;
    void* data;  // 
};


}