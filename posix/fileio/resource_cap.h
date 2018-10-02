#pragma once
// posix headers
#include "qaio.h"

namespace wjp
{
class resource_cap{
public:
    resource_cap(uint64_t n);
    void                                reset(uint64_t n);
    bool                                acquire(uint64_t n=1);
    void                                release(uint64_t n=1);
private:
    std::atomic<uint64_t>               nr_;
};




}