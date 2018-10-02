#pragma once
// posix headers
#include "qaio.h"
namespace wjp
{
    
template <uint32_t BUFSIZE> 
class membuf_template
{
public:
    membuf_template() : pos_(0){}
    bool                                append(const char* data, uint32_t size)
    {
        if(size>BUFSIZE) return false;
        auto before=pos_.fetch_add(size);
        if(before+size>=BUFSIZE){
            pos_.fetch_sub(size);
            return false;
        }
        memcpy(buf_+before, data, size);
        return true;
    }
    const char*                         data() const noexcept{
        return buf_;
    }
    bool                                empty() const noexcept{
        return pos_.load()==0;
    }
    uint32_t                            length() const noexcept{
        return pos_.load();
    }
    void                                reset() noexcept{
        pos_.store(0);
    }
private:
    char                                buf_[BUFSIZE];
    std::atomic<uint32_t>               pos_;               
};

using huge_membuf  =   membuf_template<4194304>;  //4MB
using large_membuf =   membuf_template<524288>;   //512KB
using membuf       =   membuf_template<65536>;    //64KB
using small_membuf =   membuf_template<8192>;     //8KB
using mini_membuf  =   membuf_template<1024>;     //1KB
}