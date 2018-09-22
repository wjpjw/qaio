#!/bin/bash
cd build
rm -rf *
cmake -GNinja -DCMAKE_PREFIX_PATH=/home/workspace/Libraries/tbb/ ..
ninja