#include "reader.h"
#include <unistd.h>
#include <fcntl.h>          // fcntl, posix_fadvise

namespace wjp{

reader::reader(str fname) : fname_(fname)
{
    fd_ = open(fname.c_str(), O_RDONLY);
    if(fd_<0) throw qerror("cannot open "+fname);
    posix_fadvise(fd_, 0, 0, POSIX_FADV_SEQUENTIAL);
}

reader::~reader(){close(fd_);} 

void reader::read_str(char* user_buf, size_t len)
{
    while (true) {
        auto bytes_read = read(fd_, user_buf, len);
        if (bytes_read < 0) {
            if (errno == EINTR) continue;  
            throw qerror("cannot read "+fname_);    
        }
        break;
    }
}

void reader::skip(uint64_t n)
{
    if (lseek(fd_, n, SEEK_CUR) == static_cast<off_t>(-1)) 
        throw qerror("cannot lseek "+fname_);
}


}