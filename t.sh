#!/bin/sh
#compilation and run for testing
#fno-builtin-memset asks compiler do not optimize memset() calls
#lcrypto is for linking against OpenSSL crypto library, lm - against math library
#add -Wall to see all compiler warnings
gcc test.c lib/hd_int_uniform.c -o htest -fno-builtin-memset -lcrypto -lm #-Wall
./htest
