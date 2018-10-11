#include "eventfd.h"
#include <sys/eventfd.h>
#include <unistd.h>

namespace wjp{

int efd_open()
{
    int fd;
    if(fd=event_fd(0, EFD_CLOEXEC)<0){
        throw qerror("cannot create eventfd");
    }
    fcntl(fd, F_SETFD, FD_CLOEXEC);
    fcntl(fd, F_SETFL, O_NONBLOCK);
}

void efd_send(int efd)
{
    uint64_t one=1;
    write(wfd,&one,sizeof(one));
}

void efd_recv(int efd)
{
    uint64_t duh;
    read(efd, &duh, sizeof(duh));
}


}