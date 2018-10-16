#pragma once

namespace wjp{

/*
    std::thread有几个缺陷：
        1. 默认id不是数值类型，没有办法用诸如-1的值代表不存在
        2. init即执行，没办法先定义，以后再run——除非用指针先指向nullptr，std::function设定好，far from graceful
        3. 虽然跨平台，但不可拓展，需要用tbb作底层，那就需要新的封装了。

    因此重造了轮子，tbb的封装在未来计划中，目前先用pthread迅速实现。
    
    thread的复杂度低，没必要专门在platform层封装(有的项目，比如redis根本没做封装，就直接用了pthread)，因此直接在core层调用底层代码（pthread或tbb），仍然是现代linux优先。
*/

//三种mutex，pthread只支持这三种，tbb支持更多。

//普通的mutex
class mutex{
public:
    mutex(){ //linux下destroy无必要，init不会出错
        pthread_mutex_init(&mtx, nullptr);
    }
    void    lock(){
        if(pthread_mutex_lock(m) != 0) throw qerror("mutex lock failed");
    }
    void    unlock(){
        if(pthread_mutex_unlock(m) != 0) throw qerror("mutex unlock failed");

    }
private:
    pthread_mutex_t mtx;
};

class lock{
public:
    explicit lock(mutex&mtx){
        mtx.lock();
    }
    ~lock(){mtx.unlock();}
};

//依靠用户态busy spin来阻塞的自旋锁，适用于加锁代码极少（几个时钟周期就结束）的情景，节省了通过syscall阻塞的开销
class spin_mutex{
//@todo
};

//读写锁，
class rw_mutex{
//@todo
};

//convar本质上是同步原语
class convar{
public:
    
private:
    pthread_cond_t cv_;
    mutex* mtx_;
};


class thread{
public:
    bool        is_current_thread()
    {

    }

private:
    pthread_t thr_;

};


}