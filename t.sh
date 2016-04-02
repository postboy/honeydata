#!/bin/sh
#compilation and run for testing
#fno-builtin-memset asks compiler do not optimize memset() calls
#lcrypto is for linking against OpenSSL crypto library, lm - against math library
#Wall to see all compiler warnings
gcc test_uint8_uniform.c test_common.c hdata/hd_int_uniform.c -o uint8_test -fno-builtin-memset -lcrypto -lm -Wall
./uint8_test
