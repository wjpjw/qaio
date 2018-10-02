#pragma once
// posix headers
#include "qaio.h"

namespace wjp
{
extern void                             dir_children(str dir, std::vector<std::string>& children);
extern int                              fd_limit();
extern uint64_t                         file_size(str fname);
extern bool                             is_regular_file(str fname);
extern void                             truncate_file(str fname, uint64_t bytes);
extern void                             create_dir(str dir);
extern void                             delete_dir(str dir);
extern bool                             file_exists(str fname);
extern void                             delete_file(str fname);
extern void                             rename_file(str src, str target);
}