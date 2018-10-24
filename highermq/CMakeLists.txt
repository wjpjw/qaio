aux_source_directory(. DIRSRCS)
add_library(reactor ${DIRSRCS})
target_link_libraries(highermq transport service async reactor posix utils)
