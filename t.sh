#!/bin/sh
#compilation and run for testing

#lcrypto is for linking against OpenSSL crypto library, g to produce debug information
#Wall to see all compiler warnings
#gcc tests/fp_uniform/double.c tests/t_common.c hdata/hd_fp_uniform.c hdata/hd_common.c \
#	-o build/fp_uniform/double -O0 -g -lcrypto -lm -Wall -Wfatal-errors
#cd build/fp_uniform &&
#./double
#cd ../..

gcc tests/int_uniform/uint64.c tests/t_common.c hdata/hd_int_uniform.c hdata/hd_common.c \
	-o build/int_uniform/uint64 -O0 -g -lcrypto -lgmp -Wall -Wfatal-errors
#gcc tests/int_uniform/int64.c tests/t_common.c hdata/hd_int_uniform.c hdata/hd_common.c \
#	-o build/int_uniform/int64 -O0 -g -lcrypto -lgmp -Wall -Wfatal-errors
cd build/int_uniform &&
./uint64
#./int64
cd ../..
