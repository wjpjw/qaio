#pragma once

namespace wjp{

/*
    pthread based thread abstraction
*/


class mutex{
public:
    mutex() noexcept;
    void    lock();
    void    unlock();
private:
    pthread_mutex_t mtx;
};

class lock{
public:
    explicit lock(mutex&mtx){mtx.lock();}
    ~lock(){mtx.unlock();}
};

class convar{
public:
    
private:
    pthread_cond_t cv_;
    mutex* mtx_;
};

class thread{
public:
    thread(std::function<void(void)> thread_func);
    ~thread(){
        /*
            A detached thread won't release its resource till program termniation.
        */
        pthread_join(thr_, nullptr);
    }
    bool        is_current_thread()
    {
        return pthread_self() == thr_;
    }

private:
    pthread_t thr_;

};



//@todo
//busy spin in user space
class spin_mutex{
//@todo
};

class rw_mutex{
//@todo
};


}