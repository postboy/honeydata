/*
common components of library
license: BSD 2-Clause
*/

#include "hd_common.h"

//parameters in following generic functions:
//itype - type of input elements

//generic function for finding minimum and maximum in array----------------------------------------

#define GET_ARRAY_MINMAX(itype) \
(const itype *array, const size_t size, itype *min, itype *max) \
{ \
	\
	/*check the arguments*/ \
	if (array == NULL) { \
		error("array = NULL"); \
		return 1; \
		} \
	if (size == 0) { \
		error("size = 0"); \
		return 2; \
		} \
	if (min == NULL) { \
		error("min = NULL"); \
		return 3; \
		} \
	if (max == NULL) { \
		error("max = NULL"); \
		return 4; \
		} \
	\
	itype tmpmin, tmpmax;	/*variables for storing temporary minimum and maximum values*/ \
	size_t i; \
	\
	/*initialize minimum and maximum values*/ \
	tmpmin = array[0]; \
	tmpmax = array[0]; \
	/*let's try to find smaller minimum and bigger maximum*/ \
	for (i = 1; i < size; i++) { \
		if (array[i] < tmpmin)		/*then it's the new minimum*/ \
			tmpmin = array[i]; \
		if (array[i] > tmpmax)		/*then it's the new maximum*/ \
			tmpmax = array[i]; \
		/*we don't break the cycle if ((tmpmin == itype_MIN) && (tmpmax == itype_MAX)) because it \
		can be used for timing attack; same with not proceeding to next iteration if we found a \
		new minimum (same element can't be both minimum and maximum if it is not the first)*/ \
		} \
	\
	/*finally copy results to output buffers*/ \
	*min = tmpmin; \
	*max = tmpmax; \
	\
	return 0; \
}

extern int get_uint8_minmax
	GET_ARRAY_MINMAX(uint8_t)

extern int get_int8_minmax
	GET_ARRAY_MINMAX(int8_t)

extern int get_uint16_minmax
	GET_ARRAY_MINMAX(uint16_t)

extern int get_int16_minmax
	GET_ARRAY_MINMAX(int16_t)

extern int get_uint32_minmax
	GET_ARRAY_MINMAX(uint32_t)

extern int get_int32_minmax
	GET_ARRAY_MINMAX(int32_t)

extern int get_uint64_minmax
	GET_ARRAY_MINMAX(uint64_t)

extern int get_int64_minmax
	GET_ARRAY_MINMAX(int64_t)

extern int get_float_minmax
	GET_ARRAY_MINMAX(float)

extern int get_double_minmax
	GET_ARRAY_MINMAX(double)

extern int get_longd_minmax
	GET_ARRAY_MINMAX(long double)

#undef GET_ARRAY_MINMAX

//random data generation---------------------------------------------------------------------------

//Windows-only
#ifdef WIN32

extern void randombytes(unsigned char *x, unsigned long long xlen)
{
	//initialize CryptoAPI only once
	if (hCryptProv != -1)
		if (CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, 0))
			printf("CryptAcquireContext succeeded.\n");
		else {
			printf("CryptAcquireContext() error: %x\n", GetLastError());
			return;
			}

	if (CryptGenRandom(hCryptProv, xlen, *x) == 0) {
		printf("CryptGenRandom() error: %x\n", GetLastError());
		return;
		};

	}

//*nix-only
#else

/*
Original: http://hyperelliptic.org/nacl/nacl-20110221.tar.bz2, file /randombytes/devurandom.c from
NaCl library version 20080713
*/

static int fd = -1;

extern void randombytes(unsigned char *x, unsigned long long xlen)
{
	int i;

	if (fd == -1) {
		for (;;) {
			fd = open("/dev/urandom", O_RDONLY);
			if (fd != -1) break;
			sleep(1);
			}
		}

	while (xlen > 0) {
		if (xlen < 1048576) i = xlen; else i = 1048576;

		i = read(fd,x,i);
		if (i < 1) {
			sleep(1);
			continue;
			}

		x += i;
		xlen -= i;
	}
}

#endif
