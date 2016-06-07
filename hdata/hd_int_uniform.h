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
extern int8_t get_uint8_minmax(const uint8_t *array, const size_t size,
	uint8_t *min, uint8_t *max);
extern int8_t encode_uint8_uniform(const uint8_t *in_array, uint16_t *out_array,
	const size_t size, const uint8_t min, const uint8_t max);
extern int8_t decode_uint8_uniform(const uint16_t *in_array, uint8_t *out_array,
	const size_t size, const uint8_t min, const uint8_t max);

//for signed 8 bit integers
extern int8_t get_int8_minmax(const int8_t *array, const size_t size,
	int8_t *min, int8_t *max);
extern int8_t encode_int8_uniform(const int8_t *in_array, uint16_t *out_array,
	const size_t size, const int8_t min, const int8_t max);
extern int8_t decode_int8_uniform(const uint16_t *in_array, int8_t *out_array,
	const size_t size, const int8_t min, const int8_t max);

//for unsigned and signed 16 bit integers
extern int8_t get_uint16_minmax(const uint16_t *array, const size_t size,
	uint16_t *min, uint16_t *max);
extern int8_t encode_uint16_uniform(const uint16_t *in_array, uint32_t *out_array,
	const size_t size, const uint16_t min, const uint16_t max);
extern int8_t decode_uint16_uniform(const uint32_t *in_array, uint16_t *out_array,
	const size_t size, const uint16_t min, const uint16_t max);

extern int8_t get_int16_minmax(const int16_t *array, const size_t size,
	int16_t *min, int16_t *max);
extern int8_t encode_int16_uniform(const int16_t *in_array, uint32_t *out_array,
	const size_t size, const int16_t min, const int16_t max);
extern int8_t decode_int16_uniform(const uint32_t *in_array, int16_t *out_array,
	const size_t size, const int16_t min, const int16_t max);

//for unsigned and signed 32 bit integers
extern int8_t get_uint32_minmax(const uint32_t *array, const size_t size,
	uint32_t *min, uint32_t *max);
extern int8_t encode_uint32_uniform(const uint32_t *in_array, uint64_t *out_array,
	const size_t size, const uint32_t min, const uint32_t max);
extern int8_t decode_uint32_uniform(const uint64_t *in_array, uint32_t *out_array,
	const size_t size, const uint32_t min, const uint32_t max);

extern int8_t get_int32_minmax(const int32_t *array, const size_t size,
	int32_t *min, int32_t *max);
extern int8_t encode_int32_uniform(const int32_t *in_array, uint64_t *out_array,
	const size_t size, const int32_t min, const int32_t max);
extern int8_t decode_int32_uniform(const uint64_t *in_array, int32_t *out_array,
	const size_t size, const int32_t min, const int32_t max);

#endif
