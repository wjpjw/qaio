#pragma once 

#include "qaio.h"
#include "thread.h"
#include <queue>

namespace wjp{

/*
qaio's implementation of asynchronized workflow
    a) liaison & async_job: liaison is the broker class that bridges consumer and provider; it maintains a list of async_jobs. 
    b) thread_pool & worker threads: thread_pool is a set of background worker threads that provide productivity
    c) consumer's thread: responsible for set up liaison & start an asynchronized job. After starting a job, the consumer can handle other things first, and then wait for the completion of previously started job.   
*/

/* 
An async_job is an object shared by both consumer & provider threads. 
It has:
    a) an on_finish callback: called after provider completes its job; set finish=true when this callback returns
    b) an on_cancel callback: called on cancel_job() or job expiration(if timeout is not infinite); it should tear down the job and roll back its side-effects

*/

class async_job{
public:
    async_job(func on_finish, func on_cancel):cv(mtx), finish(false), expire(0), close(false), 
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

class iov{

};

class payload{
public:
    virtual int nr_iov()=0;
    virtual iov* get_iov()=0;
};

/*
    liaison maintains a background thread that specifily handles expiration 
*/
class liaison{
public:
    //std::shared_ptr<async_job> is auto-deallocated if it is no longer refed by any party.
    std::shared_ptr<async_job> create_job(func on_finish, func on_cancel, time timeout); 
    
    // set expire time; right after that, enque the job, which means the job would be dispatched to a worker thread now or in near future; you can wait here or do something else before wait
    void dispatch_job(std::shared_ptr<async_job>); 
    
    // call the job's on_cancel callback  
    void cancel_job(std::shared_ptr<async_job>); 

    void wait_for(async_job& job); //block until completion of job's on_finish()

private:
    std::queue<std::shared_ptr<async_job>> jobs; 

};



}