#!/bin/bash
#build all tests

#lcrypto is for linking with OpenSSL crypto library, lgmp - with GNU MP library, lm - with math
#library, Wall to see main compiler warnings

int_opts='-lcrypto -lgmp -Wall'
int_u_files='tests/t_common.c hdata/hd_int_uniform.c hdata/hd_common.c'

gcc tests/int_uniform/math.c -o build/int_uniform/math -lm -Wall &&
gcc tests/int_uniform/uint8.c $int_u_files $int_opts -o build/int_uniform/uint8 &&
gcc tests/int_uniform/int8.c $int_u_files $int_opts -o build/int_uniform/int8 &&
gcc tests/int_uniform/uint16.c $int_u_files $int_opts -o build/int_uniform/uint16 &&
gcc tests/int_uniform/int16.c $int_u_files $int_opts -o build/int_uniform/int16 &&
gcc tests/int_uniform/uint32.c $int_u_files $int_opts -o build/int_uniform/uint32 &&
gcc tests/int_uniform/int32.c $int_u_files $int_opts -o build/int_uniform/int32  &&
gcc tests/int_uniform/uint64.c $int_u_files $int_opts -o build/int_uniform/uint64 &&
gcc tests/int_uniform/int64.c $int_u_files $int_opts -o build/int_uniform/int64 &&

int_a_files="hdata/hd_int_arbitrary.c $int_u_files"

gcc tests/int_arbitrary/uint8.c $int_a_files $int_opts -o build/int_arbitrary/uint8 &&

fp_opts="-lm $int_opts"
fp_u_files="hdata/hd_fp_uniform.c $int_a_files"

gcc tests/fp_uniform/compatibility.c -o build/fp_uniform/compatibility -Wall &&
gcc tests/fp_uniform/float.c $fp_u_files $fp_opts -o build/fp_uniform/float &&
gcc tests/fp_uniform/double.c $fp_u_files $fp_opts -o build/fp_uniform/double
