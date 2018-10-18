#include "thread.h"
#include <pthread.h>

namespace wjp{

//--------------------------------------------

mutex::mutex()noexcept{ 
/*
    on linux, we can expect that:
        a) pthread_mutex_init() never fails;
        b) pthread_mutex_destroy() does nothing
*/
    pthread_mutex_init(&mtx, nullptr);
}

void    mutex::lock(){
    qerror_if(pthread_mutex_lock(m) != 0, "mutex lock failed");
}
void    mutex::unlock(){
    qerror_if(pthread_mutex_unlock(m) != 0, "mutex unlock failed");
}


//--------------------------------------------

thread::thread(std::function<void(void)> thread_func)
{
    qerror_if(pthread_create(&thr->tid, nullptr, [=thread_func](){
        /*
        When a client sends data to a closed socket, it will receive a RST on the first try and causes a SIGPIPE if it tries again.

        SIGPIPE causes program termination by default, which is often sub-ideal. So we decide block it.
        */
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, SIGPIPE);
        (void) pthread_sigmask(SIG_BLOCK, &set, nullptr);
        thread_func();
        return nullptr;
    }, nullptr)!=0, "pthread_create failed");
}


}