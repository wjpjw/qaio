#include "preader.h"
#include <unistd.h>         // read, open, write, close, lseek, pread
#include <fcntl.h>          // fcntl, posix_fadvise

namespace wjp{

preader::preader(str fname) : fname_(fname)
{
    fd_ = open(fname.c_str(), O_RDONLY);
    if(fd_<0) throw qerror("cannot open "+fname);
    posix_fadvise(fd_, 0, 0, POSIX_FADV_RANDOM);
}

preader::~preader(){close(fd_);} 

void preader::read_str(char* user_buf, size_t len, uint64_t offset)
{
    auto r=pread(fd_, user_buf, len, static_cast<off_t>(offset));
    if(r<0) throw qerror("cannot pread "+fname_);
}



}