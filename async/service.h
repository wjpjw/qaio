#pragma once

#include "qaio.h"
#include "job.h"

namespace wjp{

// qaio's "service threads" are a class of threads binded with never-ending routines; 

// service is provided by a collection of these service threads
// @toimprove: traffic monitor & auto-scale algorithm
class service {
public:
    // cb is user-provided job handling method
    service(str name, int max, int min, std::function<void(std::shared_ptr<job>)> cb);
    void        start();
    void        post_job(std::shared_ptr<job> job_);
private:
    void       init_thread(int index);
    bool        add_one();
    void        remove_one();
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
    convar      cv;    //wait on mtx
};






}