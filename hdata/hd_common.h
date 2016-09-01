/*
common components of library
license: BSD 2-Clause
*/

#ifndef HD_COMMON_H
#define HD_COMMON_H

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

#include "poison.h"

//macros for printing error messages easily
#define error(...) fprintf (stderr, "error: %s: %i: %s\n", __func__, __LINE__, __VA_ARGS__)

//get minimum and maximum array values
//functions for unsigned and signed 8, 16 and 32 bit integer arrays
extern int8_t get_uint8_minmax(const uint8_t *array, const size_t size,
	uint8_t *min, uint8_t *max);
extern int8_t get_int8_minmax(const int8_t *array, const size_t size,
	int8_t *min, int8_t *max);

extern int8_t get_uint16_minmax(const uint16_t *array, const size_t size,
	uint16_t *min, uint16_t *max);
extern int8_t get_int16_minmax(const int16_t *array, const size_t size,
	int16_t *min, int16_t *max);

extern int8_t get_uint32_minmax(const uint32_t *array, const size_t size,
	uint32_t *min, uint32_t *max);
extern int8_t get_int32_minmax(const int32_t *array, const size_t size,
	int32_t *min, int32_t *max);

//for float floating-point number arrays
extern int8_t get_float_minmax(const float *array, const size_t size,
	float *min, float *max);

#endif
