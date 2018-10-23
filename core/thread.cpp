#include "thread.h"
#include <pthread.h>
#include <functional>
#include <signal.h>

namespace wjp{

mutex::mutex()noexcept{ 
/*
    on linux, we can expect that:
        a) pthread_mutex_init() never fails;
        b) pthread_mutex_destroy() does nothing
*/
    pthread_mutex_init(&mtx, nullptr);
}

void        mutex::lock(){
    qerror_if(pthread_mutex_lock(&mtx) != 0, "mutex lock failed");
}
void        mutex::unlock(){
    qerror_if(pthread_mutex_unlock(&mtx) != 0, "mutex unlock failed");
}


thread::thread(bool join) : join(join), routine(nullptr)
{}

thread::thread(std::function<void(void)> thread_func, bool join=true) 
    : join(join), routine(thread_func)
{
}

void        block_sigpipe()
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGPIPE);
    (void) pthread_sigmask(SIG_BLOCK, &set, nullptr);
}

// It is the function passed to pthread_create.
// std::function cannot be directly translated into C style function pointer 
// especially when it has lambda captures 
static void* thread_func_wrapper(void* thread_ptr)
{
    /*
        When a client sends data to a closed socket, it will receive a RST on the first try and a SIGPIPE on the 2nd try, which normally will termniate the program.
    */
    block_sigpipe();
    (((thread*)thread_ptr) -> get_routine())();
    return nullptr;
}

void        thread::run()
{
    if( is_running || !is_runnable() ) return;
    is_running=true;
    qerror_if(pthread_create(&thr, nullptr, thread_func_wrapper, this)!=0, "pthread_create failed");
}


thread::~thread(){
    if(join){
        // failure implies deadlock, so panic
        qpanic_if(pthread_join(thr, nullptr)!=0, "pthread_join failed");
    }
    pthread_detach(thr); 
}

bool        thread::is_current_thread()
{
    return pthread_self() == thr;
}

convar::convar(mutex& mtx):mtx_(&mtx)
{
    // Since qaio uses CLOCK_REALTIME, which is pthread cond's default clock setting, there is no need to call condattr_set
    pthread_cond_init(&cv_, nullptr); // cannot fail
}

convar::~convar(){
    qpanic_if(pthread_cond_destroy(&cv_)!=0, "pthread_cond_destroy failed");
}

void        convar::notify_all()
{
    qpanic_if(pthread_cond_broadcast(&cv_)!=0, "pthread_cond_broadcast failed");
}

void        convar::notify_one()
{
    qpanic_if(pthread_cond_signal(&cv_)!=0, "pthread_cond_signal failed");
}

void        convar::wait()
{
    qpanic_if(pthread_cond_wait(&cv_, &mtx_->mtx), "pthread_cond_wait failed");
}

void        convar::wait_for(time t)
{
    struct timespec ts = get_ts_after(t);
    int r=pthread_cond_timedwait(&cv_, &mtx_->mtx, &ts);
    if(r==0) return;
    else if(r==ETIMEDOUT || r==EAGAIN) throw qerror("pthread_cond_timedwait timeout");
    else throw qerror("pthread_cond_timedwait failed");
}

void        convar::wait_until(time t)
{
    struct timespec ts = get_ts(t);
    int r=pthread_cond_timedwait(&cv_, &mtx_->mtx, &ts);

}


}