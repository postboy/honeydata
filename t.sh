#!/bin/sh
#compilation and run for testing

#lcrypto is for linking against OpenSSL crypto library, g to produce debug information
#Wall to see all compiler warnings
gcc tests/float_test.c -o build/float_test -Wall &&
cd build &&
./float_test
cd ..
