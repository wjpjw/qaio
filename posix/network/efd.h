#pragma once

#include "qaio.h"

namespace wjp{

//eventfd是linux的新特性，过去并非所有发行版都支持，不过以后肯定会都支持的。它可以用做一个跨线程的flag，一个线程写，另一个线程可读，配合epoll实现一种类似cv的wait/notify信号机制。

//如果用pipe一样可以做到类似eventfd的效果，只不过pipe是单向的，要两个fd，一个读（consume singal）一个写（相当于raise singal），而且读的时候要加个for循环，彻底drain the pipe。

extern int      efd_open();
extern void     efd_send(int efd);
extern void     efd_recv(int efd);


}