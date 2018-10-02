#pragma once
// posix headers
#include "qaio.h"

namespace wjp
{

class writer
{
public:
    writer(str fname, bool is_appendable=true);
    ~writer();
    void                                sync();
    void                                write_str(const char* what, size_t len);
private:
    int                                 fd_;
    std::string                         fname_;
};


}