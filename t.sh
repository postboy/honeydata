#!/bin/sh
#compilation and run for testing

#lcrypto is for linking against OpenSSL crypto library, g to produce debug information
#Wall to see all compiler warnings
gcc tests/fp_uniform/float.c tests/t_common.c hdata/hd_fp_uniform.c hdata/hd_common.c \
	-o build/fp_uniform/float -lcrypto -Wall
cd build/fp_uniform &&
./float
cd ../..
