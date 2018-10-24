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
    job(func on_finish, func on_cancel):cv(mtx), finish(false), expire(0), close(false), 
    sleep(false)
    {}
private:
    bool finish;    // the shared state of its completion
    bool ready;     // before ready
    time expire;    // when to expire
    time duration;  // enque time + duration = expire time
    bool close;     // the state of being torn down
    bool sleep;     // the state of sleeping (for a while) 
    func on_finish; // called after provider completes its job; set finish=true when this callback returns 
    func on_cancel; // called on cancel_job() or job expiration(if timeout is not infinite); it should tear down the job and roll back its side-effects
    convar cv;
    mutex mtx;
    std::unique_ptr<payload> data; 
};


}