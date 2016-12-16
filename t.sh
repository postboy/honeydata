#!/bin/sh
#compilation and run for testing

#lcrypto is for linking against OpenSSL crypto library, g to produce debug information
#Wall to see all compiler warnings
#gcc tests/fp_uniform/double.c tests/t_common.c hdata/hd_fp_uniform.c hdata/hd_common.c \
#	-o build/fp_uniform/double -lcrypto -lm -Wall
#cd build/fp_uniform &&
#./double
#cd ../..

gcc tests/int_uniform/uint8.c tests/t_common.c hdata/hd_int_uniform.c hdata/hd_common.c \
	-o build/int_uniform/uint8 -lcrypto -lgmp -Wall
