#!/bin/sh
#compilation and testing of program with valgrind

./make.sh && \
cd build && \
valgrind --tool=memcheck --trace-children=yes --show-reachable=yes --track-fds=yes \
	--memcheck:leak-check=yes --memcheck:leak-resolution=high --max-stackframe=4194320 ./int8_test
#./uint8_test
cd ..
