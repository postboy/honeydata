#!/bin/sh
#compilation and testing of program with valgrind

./make.sh &&
cd build/int_uniform &&
valgrind --tool=memcheck --trace-children=yes --show-reachable=yes --track-fds=yes \
	--memcheck:leak-check=yes --memcheck:leak-resolution=high --max-stackframe=4194352 ./uint64
#int_uniform: uint8 int8 uint16 int16 uint32 int32 uint64 int64
#fp_uniform: float double
cd ../..
