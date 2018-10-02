#pragma once
// posix headers
#include "qaio.h"

namespace wjp
{

class preader{
public:
    preader(str fname);
    ~preader();
    void                                read_str(char* user_buf, size_t len, uint64_t offset);
private:
    int                                 fd_;
    std::string                         fname_;
};


}