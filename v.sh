#!/bin/sh
#testing a program with valgrind

./build.sh && \
valgrind --tool=memcheck --trace-children=yes --show-reachable=yes --track-fds=yes \
	--num-callers=32 --memcheck:leak-check=yes --memcheck:leak-resolution=high \
	--max-stackframe=4194320 ./uint8_test
