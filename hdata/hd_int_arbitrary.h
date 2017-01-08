/*
components for arbitrary distributed integers
license: BSD 2-Clause
*/

#ifndef HD_INT_ARBITRARY_H
#define HD_INT_ARBITRARY_H

#include "hd_common.h"
#include "hd_int_uniform.h"

//DTE and DTD for unsigned 8-bit integer arrays encoded in unsigned 16-bit integer arrays
extern int encode_uint8_arbitrary(const uint8_t *in_array, void **out_array,
	const size_t size, const uint8_t min, const uint8_t max, const uint32_t *weights);
extern int decode_uint8_arbitrary(const void *in_array, uint8_t *out_array,
	const size_t size, const uint8_t min, const uint8_t max, const uint32_t *weights);

#endif
