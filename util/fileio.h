#pragma once
// posix headers
#include <fcntl.h>          // fcntl, posix_fadvise
#include <dirent.h>         // readdir, opendir, closedir
#include <errno.h>          // errno
#include <sys/mman.h>       // mmap
#include <sys/resource.h>   // getrlimit 
#include <sys/stat.h>       // stat, S_ISREG
#include <sys/types.h>      // off_t, rlim_t
#include <unistd.h>         // read, open, write, close, lseek, pread
// c std headers
#include <stdlib.h>         // size_t, exit
#include <string.h>         // strchr
#include <stdint.h>         // uint32_t
// c++ std headers
#include <limits>
#include <atomic>
// quasi-aio headers
#include "error.h"


/*
    functions in fileio.h
    (1) create_dir   
    (2) delete_dir
    (3) file_size
    (4) rename_file
    (5) file_exists
    (6) dir_children
    (7) delte_file
    (8) fd_limit

    classes in fileio.h: 
    (1) single-threaded sequential blocking I/O: writer, buf_writer, reader, mmap_handle, preader
    (2) related utils: membuf, resource_cap, filelock, filelock_guard 

    syscalls:
    read, write, fdatasync, open(W/R), close, mmap, pread, fcntl(F_SETLK), etc.
*/

namespace wjp
{
extern void                             dir_children(str dir, std::vector<std::string>& children);
extern int                              fd_limit();
extern uint64_t                         file_size(str fname);
extern bool                             is_regular_file(str fname);

static inline void                      truncate_file(str fname, uint64_t bytes){
    if(truncate(fname.c_str(), static_cast<off_t>(bytes)) == -1) 
        throw quasi_error("cannot truncate "+fname);
}
static inline void                      create_dir(str dir){
    if(mkdir(dir.c_str(), 0755)!=0) throw quasi_error("cannot mkdir "+dir);
}
static inline void                      delete_dir(str dir){
    if(rmdir(dir.c_str())!=0) throw quasi_error("cannot rmdir "+dir);
}
static inline bool                      file_exists(str fname){
    return access(fname.c_str(), F_OK)==0;
}
static inline void                      delete_file(str fname){
    if(unlink(fname.c_str())!=0) throw quasi_error("cannot delete file "+fname);
}
static inline void                      rename_file(str src, str target){
    if(rename(src.c_str(), target.c_str())!=0) 
        throw quasi_error("cannot rename "+src+" to "+target);
}   

// [0] writer is the RAII wrapper of syscall write;
// it's blocking, sequential and not buffered.
// NOT thread-safe
class writer
{
public:
    writer(str fname, bool is_appendable=true) : fname_(fname)
    {
        if(is_appendable)  fd_ = ::open(fname.c_str(), O_APPEND | O_WRONLY | O_CREAT, 0644);
        else               fd_ = ::open(fname.c_str(),  O_TRUNC | O_WRONLY | O_CREAT, 0644);
        if(fd_<0) throw quasi_error("cannot open "+fname);
    }
    ~writer()
    {
        if(::close(fd_)<0){
            //todo: log close err
        }
    }
    void                                sync() // prefer fdatasync 
    {
        if(::fdatasync(fd_)<0) throw quasi_error("cannot fdatasync "+fname_);
    }
    void                                write_str(const char* what, size_t len);
private:
    int                                 fd_;
    std::string                         fname_;
};

// [1] buf_writer is the buffered version of writer
// append() usually behaves as in-memory copy; 
// flush() does the actual write().
// NOT thread-safe
template<uint32_t BUFSIZE>
class buf_writer_template
{
public:
    buf_writer_template(str fname) : writer_(fname){}
    ~buf_writer_template(){}
    void                                sync()
    {
        flush();
        writer_.sync();
    }
    void                                append(const char* what, size_t len)
    {
        size_t avail = std::min(len, BUFSIZE - pos_);
        memcpy(buf_ + pos_, what, avail);
        what += avail;
        len -= avail;
        pos_ += avail;
        if (len == 0) return;
        flush();
        if (len < BUFSIZE) {
            memcpy(buf_, what, len);
            pos_ = len;
        }
        else{
            writer_.write_str(what, len); // not using buffer
        }
    }
    void                                flush()
    {
        writer_.write_str(buf_, pos_);
        pos_=0;
    }
private:
    size_t                              pos_;
    writer                              writer_;
    char                                buf_[BUFSIZE]; 
};

using huge_buf_writer  =   buf_writer_template<4194304>;  //4MB
using large_buf_writer =   buf_writer_template<524288>;   //512KB
using buf_writer       =   buf_writer_template<65536>;    //64KB
using small_buf_writer =   buf_writer_template<8192>;     //8KB
using mini_buf_writer  =   buf_writer_template<1024>;     //1KB


// [2] wrapper of syscall read
// very aggressive readahead advice
// NOT thread-safe 
class reader{
public:
    reader(str fname) : fname_(fname)
    {
        fd_ = open(fname.c_str(), O_RDONLY);
        if(fd_<0) throw quasi_error("cannot open "+fname);
        posix_fadvise(fd_, 0, 0, POSIX_FADV_SEQUENTIAL); // the syscall, posix_fadvise() is very cheap
    }
    ~reader()
    {
         if(::close(fd_)<0){
             //todo: swallow & log  
         }
    }
    void                                read_str(char* user_buf, size_t len);
    void                                skip(uint64_t n)
    {
        if (lseek(fd_, n, SEEK_CUR) == static_cast<off_t>(-1)) 
            throw quasi_error("cannot lseek "+fname_);
    }
private:
    int                                 fd_;
    std::string                         fname_;
};

// [3] membuf: in-memory buffer
// append fails if it is full
// thread-safe & lock-free
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


// [4] caps for file descriptors, mmap_limit, nr_gpus etc 
// thread-safe & lock-free
class resource_cap{
public:
    resource_cap(uint64_t n):nr_(n){}
    void                                reset(uint64_t n){
        nr_.store(n);
    }
    bool                                acquire(uint64_t n=1);
    void                                release(uint64_t n=1){
        nr_.fetch_add(n);
    }
private:
    std::atomic<uint64_t>               nr_;
};

// [5] file lock
// (1) flock blocks other processes; 
// (2) mtx blocks calls from the same process; 
class filelock{

};

// [6] RAII guard of filelock
class filelock_guard{

};

// [7] random read via pread
// If your process is running out of file descriptors, 
// control preaders' life-cycles carefully!
// posix_fadvise() disables readahead, reading minimal 
// data on each physical read op!
class preader{
public:
    preader(str fname) : fname_(fname)
    {
        fd_ = open(fname.c_str(), O_RDONLY);
        if(fd_<0) throw quasi_error("cannot open "+fname);
        posix_fadvise(fd_, 0, 0, POSIX_FADV_RANDOM);
    }
    ~preader()
    {
         if(::close(fd_)<0){
             //todo: swallow & log  
         }
    }
    void                                read_str(char* user_buf, size_t len, uint64_t offset)
    {
        auto r=pread(fd_, user_buf, len, static_cast<off_t>(offset));
        if(r<0) throw quasi_error("cannot pread "+fname_);
    }
private:
    int                                 fd_;
    std::string                         fname_;
};

// [8] mmap
// random read via mmap + memory read + pagefault
// random write via mmap + memory write + writeback
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
    void                                remap(size_t new_len){
        auto p=mremap(mapped_, len_, new_len, MREMAP_MAYMOVE);
        if(p==MAP_FAILED) throw quasi_error("cannot mremap "+fname_);
    }
    void                                sync(bool is_blocking=false){
        int flag = is_blocking ? MS_SYNC : MS_ASYNC;
        if(msync(mapped_, len_, flag)) throw quasi_error("cannot msync "+fname_) ;
    }
    void                                disable_readahead() noexcept{
        madvise(mapped_, len_, MADV_RANDOM); 
    }
    void                                aggressive_readahead() noexcept{
        madvise(mapped_, len_, MADV_SEQUENTIAL); 
    }
    void                                normal_readahead() noexcept{
        madvise(mapped_, len_, MADV_NORMAL); // default kernel policy 
    }
    void                                willneed_readahead() noexcept{
        madvise(mapped_, len_, MADV_WILLNEED); // not immediately needed, but definitely needed in near future
    }
private:
    bool                                is_readonly_;
    size_t                              len_;
    void*                               mapped_;
    resource_cap*                       cap_;
    std::string                         fname_;
    int                                 fd_;
};



}