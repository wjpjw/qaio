#include "threadpool.h"
#include "thread.h"
#include <functional>
#include <queue>

namespace wjp{

struct threadpool:: meta {
    meta() : cond_(mtx_){}
    mutex mtx_;
    convar cond_;
    bool is_shutdown_ = false; 
    std::queue<func> tasks_;
};

explicit threadpool::threadpool(int nr_thread) : meta_(std::make_shared<meta>()) {
    for (int i = 0; i < nr_thread; i++) {
        thread([meta_]{
            meta_->mtx_.lock();
            while (!meta_->is_shutdown_) {
                if (!meta_->tasks_.empty()) {
                    auto task_func = std::move(meta_->tasks_.front());
                    meta_->tasks_.pop();
                    meta_->mtx_.unlock();
                    task_func();
                    meta_->mtx_.lock();
                } else {
                    meta_->cond_.wait();
                }
            }
        }).run(); 
    }
}


threadpool::~threadpool() {
    if ((bool) meta_) {
        {
            lock lk(meta_->mtx_);
            meta_->is_shutdown_ = true;
        }
        meta_->cond_.notify_all();
    }
}

void threadpool::execute(func &&task) {
    {
        std::lock lk(meta_->mtx_);
        meta_->tasks_.emplace(std::forward<func>(task));
    }
    meta_->cond_.notify_one();
}

}
