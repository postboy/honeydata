/*
float_test.c - tests for consistency with floating-point numbers processing in library
License: BSD 2-Clause
*/

#include <stdio.h>
#include <float.h>

/*unfortunately, implementation of floating-point arithmetic is platform-dependent. floating-point
related code in this library based on some assumptions about floating-point arithmetic
implementation on target platform. this file tests are this assumptions actually true on target
platform. if they are false then floating-point related code should be appropriately edited.

In particular we check that:
float is IEEE 754 single-precision binary floating-point format (binary32)
sign bit: 1 bit, exponent width: 8 bits, significand precision: 24 bits (23 explicitly stored)
double is IEEE 754 double-precision binary floating-point format (binary64)
sign bit: 1 bit, exponent width: 11 bits, significand precision: 53 bits (52 explicitly stored)
long double is IEEE 754 extended precision format, esp. x86 Extended Precision Format
sign bit: 1 bit, exponent width: 15 bits, significand precision: 64 bits (64 explicitly stored)
as wikipedia notes, this type is sometimes stored as 12 or 16 bytes to maintain data structure
alignment*/

#define CHECK(CONDITION) \
	if (CONDITION) { \
		printf("Error: " #CONDITION "\n"); \
		return 1; \
		}

extern int main(void)
{

	CHECK(FLT_RADIX != 2)
	
	CHECK(FLT_MANT_DIG != 24)
	CHECK(sizeof(float) != 4)
	
	CHECK(DBL_MANT_DIG != 53)
	CHECK(sizeof(double) != 8)
	
	CHECK(LDBL_MANT_DIG != 64)
	CHECK(sizeof(long double) != 12)
	
	//else all tests above succeded
	printf("All tests passed\n");
	return 0;

}

#undef CHECK
