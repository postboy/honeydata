#!/bin/sh
#compilation and run for testing

#lcrypto is for linking against OpenSSL crypto library, g to produce debug information
#Wall to see all compiler warnings
gcc tests/fp_uniform/compatibility.c -o build/fp_uniform/compatibility -Wall &&
cd build/fp_uniform &&
./compatibility
cd ../..
