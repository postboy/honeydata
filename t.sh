#!/bin/sh
#compilation and run for testing

#fno-builtin-memset asks compiler do not optimize memset() calls
#lcrypto is for linking against OpenSSL crypto library, lm - against math library
#g to produce debug information, Wall to see all compiler warnings
gcc tests/int32_uniform_test.c tests/test_common.c hdata/hd_int_uniform.c -o build/int32_test \
	-lcrypto -lm -g -Wall &&
cd build &&
./int32_test
cd ..
