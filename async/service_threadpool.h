#pragma once

#include "qaio.h"
#include "job.h"

namespace wjp{

// qaio's "service threads" are a class of threads binded with never-ending routines; 

// service is provided by a collection of these service threads
// @toimprove: traffic monitor & auto-scale algorithm
class service {
public:
    service(str name, int max, int min, func cb) 
        :   cv(mtx), name(name), max_nr_thread_replicates(max), 
            min_nr_thread_replicates(min), thread_cb(cb),
            cur_nr_thread_replicates(min), threads(min, std::make_shared<thread>(false))
    {
        for(int i=0; i<threads.size(); i++)
        {
            init_thread(i);
        }

    }

    void        init_thread(int index)
    {
        threads[index].thread_->set([this, index]{
            while ( !threads[index].is_shutdown ) {
                mtx.lock(); 
                if (jobs.empty()) {
                    auto job = jobs.front();
                    jobs.pop();
                    meta_->mtx_.unlock();
                    thread_cb(job)
                    meta_->mtx_.lock();
                } else {
                    meta_->cond_.wait();
                }
            }
        });

    }
    
    void        start()
    {
        for(auto& t: threads){
            t.thread_->run();
        }
    }

    bool        add_one()
    {
        lock lk(mtx2);
        int index=threads.size();
        if(index==max_nr_thread_replicates) return false;
        threads.emplace_back({false, std::make_shared<thread>(false));
        init_thread(index);
        threads[index].thread_->run();
        return true;
    }

    void        remove_one()
    {
        lock lk(mtx2);
        if(threads.size()==min_nr_thread_replicates) return false;
        int index=threads.size()-2;
        threads[index].is_shutdown=true;
        threads.erase(threads.rbegin());
    }

    // if too many jobs pended, add one 
    void        post_job(std::shared_ptr<job> job_)
    {

    }

private:
    struct service_thread{
        bool is_shutdown=false;
        std::shared_ptr<thread> thread_=nullptr;
    };
    std::queue<std::shared_ptr<job>> jobs;
    std::vector<service_thread> threads;
    std::string name; //must be unique; used in service registration  
    int         max_nr_thread_replicates;    
    int         min_nr_thread_replicates;     
    int         cur_nr_thread_replicates;  // init val = min   
    std::function<void(std::shared_ptr<job>)> thread_cb; 
    mutex       mtx;   //protect jobs
    mutex       mtx2;  //protect threads
    convar      cv;
};



// service_threadpool does pooling on exclusively service threads 
class service_threadpool{
protected:
    // @todo add limits to total nr of threads
    service_threadpool(){}
public:
    // global single instance
    static service_threadpool& me() 
    {
        static service_threadpool instance_;
        return instance_;
    }

    void unregister_service(str name)
    {
        service_lookup.erase(name);
    }

    // overwrite value if key exists
    void register_service(std::shared_ptr<service> service_)
    {
        service_lookup[name]=service_;
    }

    std::shared_ptr<service> get_service(str name)
    {
        if(service_lookup.count(name)){
            return service_lookup[name];
        }
    }

    void start_all()
    {
        for(auto it = service_lookup.begin();it!=service_lookup.end();it++){
            it->second->start();
        }
    }

private:
    std::unordered_map<std::string, std::shared_ptr<service>> service_lookup;
};


}