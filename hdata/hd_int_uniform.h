/*
hd_int_uniform.h - components for integers distributed uniformly
License: BSD 2-Clause
*/

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
extern int8_t get_uint8_array_minmax(const uint8_t *array, const uint64_t size,
	uint8_t *min, uint8_t *max);
extern int8_t encode_uint8_uniform(const uint8_t *in_array, uint16_t *out_array,
	const uint8_t min, const uint8_t max, const uint64_t size);
extern int8_t decode_uint8_uniform(const uint16_t *in_array, uint8_t *out_array,
	const uint8_t min, const uint8_t max, const uint64_t size);
