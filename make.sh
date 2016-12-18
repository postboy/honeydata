#!/bin/sh
#build all tests

#lcrypto is for linking against OpenSSL crypto library, lm - against math library
#Wall to see all compiler warnings, g to produce debug information
gcc tests/int_uniform/math.c -o build/int_uniform/math -lm -Wall &&
gcc tests/int_uniform/uint8.c tests/t_common.c hdata/hd_int_uniform.c hdata/hd_common.c \
	-o build/int_uniform/uint8 -lcrypto -lgmp -Wall &&
gcc tests/int_uniform/int8.c tests/t_common.c hdata/hd_int_uniform.c hdata/hd_common.c \
	-o build/int_uniform/int8 -lcrypto -lgmp -Wall &&
gcc tests/int_uniform/uint16.c tests/t_common.c hdata/hd_int_uniform.c hdata/hd_common.c \
	-o build/int_uniform/uint16 -lcrypto -lgmp -Wall &&
gcc tests/int_uniform/int16.c tests/t_common.c hdata/hd_int_uniform.c hdata/hd_common.c \
	-o build/int_uniform/int16 -lcrypto -lgmp -Wall &&
gcc tests/int_uniform/uint32.c tests/t_common.c hdata/hd_int_uniform.c hdata/hd_common.c \
	-o build/int_uniform/uint32 -lcrypto -lgmp -Wall &&
gcc tests/int_uniform/int32.c tests/t_common.c hdata/hd_int_uniform.c hdata/hd_common.c \
	-o build/int_uniform/int32 -lcrypto -lgmp -Wall &&
gcc tests/int_uniform/uint64.c tests/t_common.c hdata/hd_int_uniform.c hdata/hd_common.c \
	-o build/int_uniform/uint64 -lcrypto -lgmp -Wall &&

gcc tests/fp_uniform/compatibility.c -o build/fp_uniform/compatibility -Wall  &&
gcc tests/fp_uniform/float.c tests/t_common.c hdata/hd_fp_uniform.c hdata/hd_common.c \
	-o build/fp_uniform/float -lcrypto -lgmp -lm -Wall &&
gcc tests/fp_uniform/double.c tests/t_common.c hdata/hd_fp_uniform.c hdata/hd_common.c \
	-o build/fp_uniform/double -lcrypto -lgmp -lm -Wall
