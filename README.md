# 开发笔记 
## 2018-9 

### 我想写什么
1. 读[nng](https://github.com/nanomsg/nng)源码有感，aio与task机制实现得仍显繁琐，缺乏建筑美感，想要用现代c++重写。
2. 此外，并发库打算用[tbb](https://github.com/01org/tbb)而避免直接用pthread。nng的线程池线程数量严格等于nr_cpu×2（因为intel超线程），但这并非最优配置。很多时候，任务载荷并不完全被cpu所消耗（而是其他设备，尤其是网络），假如有一千个阻塞操作都只是向服务端发送请求然后等返回，那么这时线程无疑是越多越好的。tbb不仅提供fine-grained locking，还有更好的task抽象，想试试看写个更好的底层任务队列。
3. 受[leveldb](https://github.com/google/leveldb)启发，打算把posix的文件IO封装得现代一点，好用一点。leveldb在文件IO的读写性能上还有优化空间（都是单线程+阻塞），打算把这方面做到极致（比如多重缓冲加非阻塞写）。
4. c/c++的MQ往往专注性能，欠缺完整支撑，我想有空拜读下[rocketmq](https://github.com/apache/rocketmq)等业务支撑到位的MQ，看看他山之石可否攻玉。
5. 最终目标是在Linux（当然很容易移植到Unix-like）上搭建一个由现代c++实现的系统编程框架——在此基础上可以方便地构筑异步网络服务器、持久化和无持久化的MQ、高性能的日志系统、灾备、变更捕获与数据同步。

### 开发环境
1. Mac上用ubuntu（docker容器）做调试系统，建好容器以后每次只需docker container attach，很方便。
2. 用VS Code作编辑器，项目文件夹直连ubuntu容器。
3. 用CMake+Ninja构建项目。
4. 用现代c++，完全不用boost或其他库，非并发前提下用stl。
5. 目前只考虑tbb作为依赖。






