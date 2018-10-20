#pragma once

#include "qaio.h"
#include "time.h"

namespace wjp{

/*
    pthread based thread abstraction
*/
class convar;
class mutex{
public:
    mutex() noexcept;
    void    lock();
    void    unlock();
private:
    friend class convar;
    pthread_mutex_t mtx;
};

class lock{
public:
    explicit lock(mutex&mtx):mtx(mtx){mtx.lock();}
    ~lock(){mtx.unlock();}
private:
    mutex& mtx;
};

class convar{
public:
    convar(mutex&mtx);
    ~convar();
    void        notify_all();
    void        notify_one();
    void        wait();
    void        wait_for(time);
    void        wait_until(time);
private:
    pthread_cond_t cv_;
    mutex* mtx_;
};

/*
    A wjp::thread does not immediately start the thread routine. 
    You can prepare it, store it somewhere, and wait for the correct time to start it.
*/
class thread{
public:
    thread(bool join=true);
    thread(std::function<void(void)>, bool);
    ~thread();
    void        set(std::function<void(void)>);
    void        run();
    bool        is_current_thread()
    {
        return pthread_self() == thr;
    }
    bool        is_runnable(){
        return routine!=nullptr;
    }
    std::function<void(void)>& get_routine()
    {
        return routine;
    }
private:
    pthread_t thr;
    bool join;
    bool is_running;
    std::function<void(void)> routine;
};

extern void block_sigpipe();
}