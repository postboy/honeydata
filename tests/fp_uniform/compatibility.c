/*
tests for compatibility with floating-point numbers processing in library
license: BSD 2-Clause
*/

#include <stdio.h>
#include <inttypes.h>
#include <float.h>

/*unfortunately, implementation of floating-point arithmetic is platform-dependent. floating-point
related code in this library based on some assumptions about floating-point arithmetic
implementation on target platform. this file tests are this assumptions actually true on target
platform. if they are false then floating-point related code should be appropriately edited.

in particular we check that:
- float is IEEE 754 single-precision binary floating-point format (binary32)
sign bit: 1 bit, exponent width: 8 bits, significand precision: 24 bits (23 explicitly stored)
- double is IEEE 754 double-precision binary floating-point format (binary64)
sign bit: 1 bit, exponent width: 11 bits, significand precision: 53 bits (52 explicitly stored)
- long double is IEEE 754 extended precision format, esp. x86 Extended Precision Format
sign bit: 1 bit, exponent width: 15 bits, significand precision: 64 bits (64 explicitly stored)
as wikipedia notes, this type is sometimes stored as 12 or 16 bytes to maintain data structure
alignment

author's PC uses little-endian encoding*/

#define CHECK(CONDITION) \
	do { \
		if (CONDITION) { \
			printf("Error: " #CONDITION "\n"); \
			return 1; \
			} \
		} while (0)

extern int main(void)
{

	int i;

	CHECK(FLT_RADIX != 2);
	
	CHECK(FLT_MANT_DIG != 24);
	CHECK(sizeof(float) != 4);
	
	CHECK(DBL_MANT_DIG != 53);
	CHECK(sizeof(double) != 8);
	
	/*
	CHECK(LDBL_MANT_DIG != 64);
	CHECK(sizeof(long double) != 12);
	*/
	
	union {
		float fl;
		uint32_t i;
		unsigned char ch[4];
		} hfloat;
	
	hfloat.fl = -0.15625;
	
	uint16_t sexp1 = hfloat.i >> 23;
	uint32_t frac1 = hfloat.i & 0x007FFFFF;
	
	CHECK(sexp1 != 380);
	CHECK(frac1 != 2097152);
	for (i = 0; i < sizeof(float); i++)
		printf("%02x", hfloat.ch[i]);	//output format is HEX-code
	//my output: 000020be
	printf("\n");
	
	union {
		double fl;
		uint64_t i;
		unsigned char ch[8];
		} hdouble;
	
	hdouble.fl = -0.15625;
	
	uint16_t sexp2 = hdouble.i >> 52;
	uint64_t frac2 = hdouble.i & 0x000FFFFFFFFFFFFF;
	
	CHECK(sexp2 != 3068);
	CHECK(frac2 != 1125899906842624);
	for (i = 0; i < sizeof(double); i++)
		printf("%02x", hdouble.ch[i]);
	//my output: 000000000000c4bf
	printf("\n");
	
	/*
	union {
		long double fl;
		uint64_t i[2];
		unsigned char ch[12];
		} hlongd;
	
	hlongd.fl = -0.15625;
	
	uint16_t sexp3 = hlongd.i[0] & 0x000000000000FFFF;
	uint64_t frac3 = hlongd.i[1];
	
	//CHECKs go here
	for (i = 0; i < sizeof(long double); i++)
		printf("%02x", hlongd.ch[i]);
	//my output: 00000000000000a0fcbf61b7
	printf("\n%"PRIu64" %"PRIu64"\n", hlongd.i[0], hlongd.i[1]);
	*/
	
	/*
	__fp16
	__float80/128
	_Decimal32/64/128
	_Complex int/float/double/long double
	*/
	
	//else all tests above succeeded
	printf("All tests passed\n");
	return 0;

}

#undef CHECK
