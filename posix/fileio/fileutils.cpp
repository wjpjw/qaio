#include "fileutils.h"
#include <fcntl.h>          // fcntl, posix_fadvise
#include <dirent.h>         // readdir, opendir, closedir
#include <sys/resource.h>   // getrlimit 
#include <sys/stat.h>       // stat, S_ISREG
#include <unistd.h>         // read, open, write, close, lseek, pread

namespace wjp{

int fd_limit()
{
    struct rlimit rlim;
    if(getrlimit(RLIMIT_NOFILE, &rlim)){
        throw qerror("cannot getrlimit");
    }else if(rlim.rlim_cur==RLIM_INFINITY){
        return std::numeric_limits<int>::max();
    }
    else{
        return rlim.rlim_cur;
    }
}

void dir_children(const std::string& dir, std::vector<std::string>& children)
{
    children.clear();
    DIR* d=opendir(dir.c_str());
    if(d==nullptr) throw qerror("cannot opendir");
    struct dirent* entry;
    while((entry=readdir(d))!=nullptr){
        children.emplace_back(entry->d_name);
    }
    closedir(d);
}

uint64_t file_size(const std::string& fname)
{
    struct stat sbuf;
    if (stat(fname.c_str(), &sbuf) != 0) {
        throw qerror("cannot stat "+fname);
    } else {
        return sbuf.st_size;
    }
}

bool is_regular_file(str fname)
{
    struct stat sbuf;
    if (stat(fname.c_str(), &sbuf) != 0) {
        throw qerror("cannot stat "+fname);
    } else {
        return S_ISREG(sbuf.st_mode);
    }
}

void truncate_file(str fname, uint64_t bytes){
    if(truncate(fname.c_str(), static_cast<off_t>(bytes)) == -1) 
        throw qerror("cannot truncate "+fname);
}

void create_dir(str dir){
    if(mkdir(dir.c_str(), 0755)!=0) throw qerror("cannot mkdir "+dir);
}

void delete_dir(str dir){
    if(rmdir(dir.c_str())!=0) throw qerror("cannot rmdir "+dir);
}

bool file_exists(str fname){
    return access(fname.c_str(), F_OK)==0;
}

void delete_file(str fname){
    if(unlink(fname.c_str())!=0) throw qerror("cannot delete file "+fname);
}

void rename_file(str src, str target){
    if(rename(src.c_str(), target.c_str())!=0) 
        throw qerror("cannot rename "+src+" to "+target);
}   


}