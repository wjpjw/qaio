#pragma once
// posix headers
#include "qaio.h"
#include "resource_cap.h"

namespace wjp
{

class mmap_handle{
public:    
    mmap_handle(str fname, resource_cap* cap, bool is_readonly=true);
    ~mmap_handle();
    char*                               base() noexcept{
        return reinterpret_cast<char*>(mapped_);
    }
    char*                               locate(uint64_t offset) noexcept{
        return reinterpret_cast<char*>(mapped_)+offset;
    }
    bool                                is_readonly() noexcept{
        return is_readonly_;
    }
    void                                remap(size_t new_len);
    void                                sync(bool is_blocking=false);
    void                                disable_readahead() noexcept;
    void                                aggressive_readahead() noexcept;
    void                                normal_readahead() noexcept;
    void                                willneed_readahead() noexcept;
private:
    bool                                is_readonly_;
    size_t                              len_;
    void*                               mapped_;
    resource_cap*                       cap_;
    std::string                         fname_;
    int                                 fd_;
};


}