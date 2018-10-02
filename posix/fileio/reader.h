#pragma once
// posix headers
#include "qaio.h"

namespace wjp
{
    
class reader{
public:
    reader(str fname);
    ~reader();
    void                                read_str(char* user_buf, size_t len);
    void                                skip(uint64_t n);
private:
    int                                 fd_;
    std::string                         fname_;
};

}