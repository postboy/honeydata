#!/bin/sh
#compilation and run for testing

#g to produce debug information, O0 to improve debugging via disabling optimization, Wfatal-errors
#to not sink in tons of warnings and errors from compiler

gcc tests/fp_uniform/float.c tests/t_common.c hdata/hd_fp_uniform.c hdata/hd_common.c \
	-o build/fp_uniform/float -O0 -g -lcrypto -lgmp -lm -Wall -Wfatal-errors
cd build/fp_uniform &&
./float
cd ../..
