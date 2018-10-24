aux_source_directory(. DIRSRCS)
add_library(async ${DIRSRCS})
target_link_libraries(async posix utils)
