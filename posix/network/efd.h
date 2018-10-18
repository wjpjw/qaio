#pragma once

#include "qaio.h"

namespace wjp{

/*
    Eventfd: Linux-specific light-weight IPC signaling

    eventfd() creates an "eventfd object" that can be used as an event wait/notify mechanism by user-space applications, and by the kernel to notify user-space applications of events. The object contains an unsigned 64-bit integer (uint64_t) counter that is maintained by the kernel.
*/
extern int      efd_open();
extern void     efd_send(int efd);
extern void     efd_recv(int efd);


}