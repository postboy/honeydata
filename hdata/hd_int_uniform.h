/*
hd_int_uniform.h - components for integers distributed uniformly
License: BSD 2-Clause
*/

#ifndef HD_INT_UNIFORM_H
#define HD_INT_UNIFORM_H

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

#include "poison.h"

//macros for printing error messages easily
#define error(...) fprintf (stderr, "error: %s: %i: %s\n", __func__, __LINE__, __VA_ARGS__)

//functions for unsigned 8 bit integers: get minimum and maximum array values, DTE and DTD
#define itype uint8_t
#define otype uint16_t
extern int8_t get_uint8_minmax(const itype *array, const uint64_t size,
	itype *min, itype *max);
extern int8_t encode_uint8_uniform(const itype *in_array, otype *out_array,
	const uint64_t size, const itype min, const itype max);
extern int8_t decode_uint8_uniform(const otype *in_array, itype *out_array,
	const uint64_t size, const itype min, const itype max);
#undef itype
#undef otype

//for signed 8 bit integers
#define itype int8_t
#define otype uint16_t
extern int8_t get_sint8_minmax(const itype *array, const uint64_t size,
	itype *min, itype *max);
extern int8_t encode_sint8_uniform(const itype *in_array, otype *out_array,
	const uint64_t size, const itype min, const itype max);
extern int8_t decode_sint8_uniform(const otype *in_array, itype *out_array,
	const uint64_t size, const itype min, const itype max);
#undef itype
#undef otype

//for unsigned and signed 16 bit integers
#define itype uint16_t
#define otype uint32_t
extern int8_t get_uint16_minmax(const itype *array, const uint64_t size,
	itype *min, itype *max);
extern int8_t encode_uint16_uniform(const itype *in_array, otype *out_array,
	const uint64_t size, const itype min, const itype max);
extern int8_t decode_uint16_uniform(const otype *in_array, itype *out_array,
	const uint64_t size, const itype min, const itype max);
#undef itype
#undef otype

#define itype int16_t
#define otype uint32_t
extern int8_t get_sint16_minmax(const itype *array, const uint64_t size,
	itype *min, itype *max);
extern int8_t encode_sint16_uniform(const itype *in_array, otype *out_array,
	const uint64_t size, const itype min, const itype max);
extern int8_t decode_sint16_uniform(const otype *in_array, itype *out_array,
	const uint64_t size, const itype min, const itype max);
#undef itype
#undef otype

//for unsigned and signed 32 bit integers
#define itype uint32_t
#define otype uint64_t
extern int8_t get_uint32_minmax(const itype *array, const uint64_t size,
	itype *min, itype *max);
extern int8_t encode_uint32_uniform(const itype *in_array, otype *out_array,
	const uint64_t size, const itype min, const itype max);
extern int8_t decode_uint32_uniform(const otype *in_array, itype *out_array,
	const uint64_t size, const itype min, const itype max);
#undef itype
#undef otype

#define itype int32_t
#define otype uint64_t
extern int8_t get_sint32_minmax(const itype *array, const uint64_t size,
	itype *min, itype *max);
extern int8_t encode_sint32_uniform(const itype *in_array, otype *out_array,
	const uint64_t size, const itype min, const itype max);
extern int8_t decode_sint32_uniform(const otype *in_array, itype *out_array,
	const uint64_t size, const itype min, const itype max);
#undef itype
#undef otype

#endif
