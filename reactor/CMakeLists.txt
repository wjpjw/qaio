aux_source_directory(. DIRSRCS)
add_library(reactor ${DIRSRCS})
target_link_libraries(reactor posix utils)
