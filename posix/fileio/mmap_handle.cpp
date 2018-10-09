#include "mmap_handle.h"
#include "fileutils.h"
#include <unistd.h>         // read, open, write, close, lseek, pread
#include <sys/mman.h>       // mmap
#include <fcntl.h>

namespace wjp{
mmap_handle::mmap_handle(const std::string& fname, resource_cap* cap, bool is_readonly) : fname_(fname), cap_(cap), is_readonly_(is_readonly)
{
    if(!cap->acquire()) throw qerror("not enough mmap quota");
    fd_=open(fname.c_str(), O_RDONLY);
    if(fd_<0) {
        cap->release();
        throw qerror("cannot open "+fname);
    }
    try{
        len_=file_size(fname);
    }catch(qerror& err){
        cap->release();
        close(fd_);
        throw;
    }
    if(is_readonly)
        mapped_=mmap(nullptr, len_, PROT_READ, MAP_SHARED, fd_, 0);
    else
        mapped_=mmap(nullptr, len_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
    if(mapped_==MAP_FAILED){
        cap->release();
        close(fd_);
        throw qerror("cannot mmap "+fname);
    }
}

mmap_handle::~mmap_handle()
{
    munmap(mapped_, len_);
    cap_->release();
    close(fd_);
}

void mmap_handle::remap(size_t new_len){
    auto p=mremap(mapped_, len_, new_len, MREMAP_MAYMOVE);
    if(p==MAP_FAILED) throw qerror("cannot mremap "+fname_);
}

void mmap_handle::sync(bool is_blocking){
    int flag = is_blocking ? MS_SYNC : MS_ASYNC;
    if(msync(mapped_, len_, flag)) throw qerror("cannot msync "+fname_) ;
}

void mmap_handle::disable_readahead() noexcept{
    madvise(mapped_, len_, MADV_RANDOM); 
}

void mmap_handle::aggressive_readahead() noexcept{
    madvise(mapped_, len_, MADV_SEQUENTIAL); 
}

void mmap_handle::normal_readahead() noexcept{
    madvise(mapped_, len_, MADV_NORMAL); // default kernel policy 
}

void mmap_handle::willneed_readahead() noexcept{
    madvise(mapped_, len_, MADV_WILLNEED); // not immediately needed, but definitely needed in near future
}

}