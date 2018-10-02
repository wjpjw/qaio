#include "resource_cap.h"

namespace wjp{
    
resource_cap::resource_cap(uint64_t n):nr_(n){}

void resource_cap::reset(uint64_t n){
    nr_.store(n);
}

void resource_cap::release(uint64_t n=1){
    nr_.fetch_add(n);
}

bool resource_cap::acquire(uint64_t n){
    auto before=nr_.fetch_sub(n); 
    if(before-n<=0){
        nr_.fetch_add(n);
        return false;
    } 
    return true;
}

}