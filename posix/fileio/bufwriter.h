#pragma once
// posix headers
#include "qaio.h"
#include "writer.h"
namespace wjp
{

template<uint32_t BUFSIZE>
class buf_writer_template
{
public:
    buf_writer_template(str fname) : writer_(fname){}
    ~buf_writer_template(){}
    void                                sync()
    {
        flush();
        writer_.sync();
    }
    void                                append(const char* what, size_t len)
    {
        size_t avail = std::min(len, BUFSIZE - pos_);
        memcpy(buf_ + pos_, what, avail);
        what += avail;
        len -= avail;
        pos_ += avail;
        if (len == 0) return;
        flush();
        if (len < BUFSIZE) {
            memcpy(buf_, what, len);
            pos_ = len;
        }
        else{
            writer_.write_str(what, len); // not using buffer
        }
    }
    void                                flush()
    {
        writer_.write_str(buf_, pos_);
        pos_=0;
    }
private:
    size_t                              pos_;
    writer                              writer_;
    char                                buf_[BUFSIZE]; 
};

using huge_buf_writer  =   buf_writer_template<4194304>;  //4MB
using large_buf_writer =   buf_writer_template<524288>;   //512KB
using buf_writer       =   buf_writer_template<65536>;    //64KB
using small_buf_writer =   buf_writer_template<8192>;     //8KB
using mini_buf_writer  =   buf_writer_template<1024>;     //1KB




}