#include "writer.h"
#include <unistd.h>
#include <fcntl.h>

namespace wjp{

writer::writer(str fname, bool is_appendable) : fname_(fname) 
{
    fd_ = open(fname.c_str(), (is_appendable ? O_APPEND : O_TRUNC) | O_WRONLY | O_CREAT, 0644);
    if(unlikely(fd_<0)) throw qerror("cannot open "+fname);
}

writer::~writer(){close(fd_);}

void writer::sync() // prefer fdatasync 
{
    if(unlikely(fdatasync(fd_)<0)) throw qerror("cannot fdatasync "+fname_);
}

void writer::write_str(const char* what, size_t len)
{
    while (len > 0) {
        auto bytes_written = ::write(fd_, what, len);
        if (bytes_written < 0) {
            if (errno == EINTR) continue; 
            throw qerror("cannot write "+fname_);
        }
        what += bytes_written;
        len -= bytes_written;
    }
}

}