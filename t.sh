#!/bin/sh
#compilation and run for testing

#g to produce debug information, O0 to improve debugging via disabling optimization, Wfatal-errors
#to not sink in tons of warnings and errors from compiler

gcc tests/int_arbitrary/uint8.c tests/t_common.c \
	hdata/hd_int_arbitrary.c hdata/hd_int_uniform.c hdata/hd_common.c \
	-o build/int_arbitrary/uint8 -O0 -g -lcrypto -lgmp -Wall -Wfatal-errors
cd build/int_arbitrary &&
./uint8
cd ../..
