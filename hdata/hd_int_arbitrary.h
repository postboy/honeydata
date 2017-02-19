/*
components for arbitrary distributed integers
license: BSD 2-Clause
*/

#ifndef HD_INT_ARBITRARY_H
#define HD_INT_ARBITRARY_H

#include "hd_common.h"
#include "hd_int_uniform.h"

//DTE and DTD for unsigned and signed 8-, 16-, 32- and 64-bit integer arrays
extern int encode_uint8_arbitrary(const uint8_t *in_array, void **out_array,
	const size_t size, const uint8_t min, const uint8_t max, const uint32_t *weights);
extern int decode_uint8_arbitrary(const void *in_array, uint8_t *out_array,
	const size_t size, const uint8_t min, const uint8_t max, const uint32_t *weights);

extern int encode_int8_arbitrary(const int8_t *in_array, void **out_array,
	const size_t size, const int8_t min, const int8_t max, const uint32_t *weights);
extern int decode_int8_arbitrary(const void *in_array, int8_t *out_array,
	const size_t size, const int8_t min, const int8_t max, const uint32_t *weights);

extern int encode_uint16_arbitrary(const uint16_t *in_array, void **out_array,
	const size_t size, const uint16_t min, const uint16_t max, const uint32_t *weights);
extern int decode_uint16_arbitrary(const void *in_array, uint16_t *out_array,
	const size_t size, const uint16_t min, const uint16_t max, const uint32_t *weights);

extern int encode_int16_arbitrary(const int16_t *in_array, void **out_array,
	const size_t size, const int16_t min, const int16_t max, const uint32_t *weights);
extern int decode_int16_arbitrary(const void *in_array, int16_t *out_array,
	const size_t size, const int16_t min, const int16_t max, const uint32_t *weights);

extern int encode_uint32_arbitrary(const uint32_t *in_array, void **out_array,
	const size_t size, const uint32_t min, const uint32_t max, const uint32_t *weights);
extern int decode_uint32_arbitrary(const void *in_array, uint32_t *out_array,
	const size_t size, const uint32_t min, const uint32_t max, const uint32_t *weights);

extern int encode_int32_arbitrary(const int32_t *in_array, void **out_array,
	const size_t size, const int32_t min, const int32_t max, const uint32_t *weights);
extern int decode_int32_arbitrary(const void *in_array, int32_t *out_array,
	const size_t size, const int32_t min, const int32_t max, const uint32_t *weights);

extern int encode_uint64_arbitrary(const uint64_t *in_array, void **out_array,
	const size_t size, const uint64_t min, const uint64_t max, const uint32_t *weights);
extern int decode_uint64_arbitrary(const void *in_array, uint64_t *out_array,
	const size_t size, const uint64_t min, const uint64_t max, const uint32_t *weights);

extern int encode_int64_arbitrary(const int64_t *in_array, void **out_array,
	const size_t size, const int64_t min, const int64_t max, const uint32_t *weights);
extern int decode_int64_arbitrary(const void *in_array, int64_t *out_array,
	const size_t size, const int64_t min, const int64_t max, const uint32_t *weights);

#endif
