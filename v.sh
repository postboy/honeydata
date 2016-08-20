#!/bin/sh
#compilation and testing of program with valgrind

./make.sh &&
cd build/int_uniform &&
valgrind --tool=memcheck --trace-children=yes --show-reachable=yes --track-fds=yes \
	--memcheck:leak-check=yes --memcheck:leak-resolution=high --max-stackframe=4194352 ./int32
#int32 uint32 int16 uint16 int8 uint8
cd ../..
