/*
components for integers distributed uniformly
license: BSD 2-Clause
*/

#ifndef HD_INT_UNIFORM_H
#define HD_INT_UNIFORM_H

#include "hd_common.h"

//DTE and DTD for unsigned and signed 8, 16 and 32 bit integer arrays
extern int8_t encode_uint8_uniform(const uint8_t *in_array, uint16_t *out_array,
	const size_t size, const uint8_t min, const uint8_t max);
extern int8_t decode_uint8_uniform(const uint16_t *in_array, uint8_t *out_array,
	const size_t size, const uint8_t min, const uint8_t max);

extern int8_t encode_int8_uniform(const int8_t *in_array, uint16_t *out_array,
	const size_t size, const int8_t min, const int8_t max);
extern int8_t decode_int8_uniform(const uint16_t *in_array, int8_t *out_array,
	const size_t size, const int8_t min, const int8_t max);

extern int8_t encode_uint16_uniform(const uint16_t *in_array, uint32_t *out_array,
	const size_t size, const uint16_t min, const uint16_t max);
extern int8_t decode_uint16_uniform(const uint32_t *in_array, uint16_t *out_array,
	const size_t size, const uint16_t min, const uint16_t max);

extern int8_t encode_int16_uniform(const int16_t *in_array, uint32_t *out_array,
	const size_t size, const int16_t min, const int16_t max);
extern int8_t decode_int16_uniform(const uint32_t *in_array, int16_t *out_array,
	const size_t size, const int16_t min, const int16_t max);

extern int8_t encode_uint32_uniform(const uint32_t *in_array, uint64_t *out_array,
	const size_t size, const uint32_t min, const uint32_t max);
extern int8_t decode_uint32_uniform(const uint64_t *in_array, uint32_t *out_array,
	const size_t size, const uint32_t min, const uint32_t max);

extern int8_t encode_int32_uniform(const int32_t *in_array, uint64_t *out_array,
	const size_t size, const int32_t min, const int32_t max);
extern int8_t decode_int32_uniform(const uint64_t *in_array, int32_t *out_array,
	const size_t size, const int32_t min, const int32_t max);

#endif
