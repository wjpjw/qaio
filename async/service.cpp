#include "service.h"
#include "job.h"

namespace wjp{


    // cb is user-provided job handling method
service::service(str name, int max, int min, std::function<void(std::shared_ptr<job>)> cb) 
    :   cv(mtx), name(name), max_nr_thread_replicates(max), 
        min_nr_thread_replicates(min), thread_cb(cb),
        cur_nr_thread_replicates(min), threads(min, std::make_shared<thread>(false))
{
    for(int i=0; i<threads.size(); i++)
    {
        init_thread(i);
    }
}

void       service::init_thread(int index)
{
    threads[index].thread_->set([this, index]{
        while ( !threads[index].is_shutdown ) {
            mtx.lock(); 
            if (jobs.empty()) {
                auto job = jobs.front();
                jobs.pop();
                mtx.unlock();
                thread_cb(job)
                mtx.lock();
            } else {
                cv.wait();
            }
        }
    });
}

void        service::start()
{
    for(auto& t: threads){
        t.thread_->run();
    }
}

bool        service::add_one()
{
    lock lk(mtx2);
    int index=threads.size();
    if(index==max_nr_thread_replicates) return false;
    threads.emplace_back({false, std::make_shared<thread>(false));
    init_thread(index);
    threads[index].thread_->run();
    return true;
}

void        service::remove_one()
{
    lock lk(mtx2);
    if(threads.size()==min_nr_thread_replicates) return false;
    int index=threads.size()-2;
    threads[index].is_shutdown=true;
    threads.erase(threads.rbegin());
}

void        service::post_job(std::shared_ptr<job> job_)
{
    {
        lock lk(mtx);
        jobs.emplace(job_);
    }
    cv.notify_one();
    //@todo: add auto-scale here 

}
}