#!/bin/sh
#compilation and run for testing
#fno-builtin-memset asks compiler do not optimize memset() calls

gcc test.c lib/hd_int_uniform.c -o htest -Wall -fno-builtin-memset -lcrypto -lssl 
./htest
