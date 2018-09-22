#include "fileio.h"
namespace wjp{
int                                     fd_limit()
{
    struct rlimit rlim;
    if(getrlimit(RLIMIT_NOFILE, &rlim)){
        throw quasi_error(errno, "cannot getrlimit");
    }else if(rlim.cur==RLIM_INFINITY){
        return std:numeric_limits<int>::max();
    }
    else{
        return rlim.rlim_cur;
    }
}

void                                    dir_children(const std::string& dir, std::vector<std::string>& children)
{
    children.clear();
    DIR* d=opendir(dir.c_str());
    if(d==nullptr) throw quasi_error(errno, "cannot opendir");
    struct dirent* entry;
    while((entry=readdir(d))!=nullptr){
        children.emplace_back(entry->d_name);
    }
    closedir(d);
}

int64_t                                 file_size(const std::string& fname)
{
    struct stat sbuf;
    if (stat(fname.c_str(), &sbuf) != 0) {
        throw quasi_error(errno, "cannot stat "+fname);
    } else {
        return sbuf.st_size;
    }
}

bool                                    is_regular_file(str fname)
{
    struct stat sbuf;
    if (stat(fname.c_str(), &sbuf) != 0) {
        throw quasi_error(errno, "cannot stat "+fname);
    } else {
        return S_ISREG(sbuf.st_mode);
    }
}


bool                                    resource_cap::acquire(uint64_t n){
    auto before=nr_.fetch_sub(n); 
    if(before-n<=0){
        nr_.fetch_add(n);
        return false;
    } 
    return true;
}

mmap_handle::mmap_handle(const std::string& fname, resource_cap* cap, bool is_readonly) : fname_(fname), cap_(cap), is_readonly_(is_readonly)
{
    if(!cap->acquire()) throw quasi_error("not enough mmap quota");
    fd_=open(fname.c_str(), O_RDONLY);
    if(fd<0) {
        cap->release();
        throw quasi_error(errno, "cannot open "+fname);
    }
    try{
        len_=file_size(fname);
    }catch(quasi_error& err){
        cap->release();
        close(fd_);
        throw;
    }
    if(is_readonly)
        mapped_=mmap(nullptr, len_, PORT_READ, MAP_SHARED, fd_, 0);
    else
        mapped_=mmap(nullptr, len_, PORT_READ | PORT_WRITE, MAP_SHARED, fd_, 0);
    if(mapped==MAP_FAILED){
        cap->release();
        close(fd_);
        throw quasi_error(errno, "cannot mmap "+fname);
    }
}

mmap_handle::~mmap_handle()
{
    munmap(mapped_, len_);
    cap_->release();
    if(::close(fd_)<0){
        //todo: swallow & log  
    }
}

void                                    reader::read_str(char* user_buf, size_t len)
{
    while (true) {
        auto bytes_read = read(fd_, user_buf, len);
        if (bytes_read < 0) {
            if (errno == EINTR) continue;  
            throw quasi_error(errno,"cannot read "+fname_);    
        }
        break;
    }
}

void                                    writer::write_str(const char* what, size_t len)
{
    while (len > 0) {
        auto bytes_written = ::write(fd_, what, len);
        if (bytes_written < 0) {
            if (errno == EINTR) continue; 
            throw quasi_error(errno, "cannot write "+fname_);
        }
        what += bytes_written;
        len -= bytes_written;
    }
}

}