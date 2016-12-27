/*
components for arbitrary distributed integers
license: BSD 2-Clause
*/

#ifndef HD_INT_ARBITRARY_H
#define HD_INT_ARBITRARY_H

#include "hd_common.h"
#include "hd_int_uniform.h"

/*convert weights to cumulative weights and determine minimum size of container for values with
such cumulative weights*/
extern int get_cumuls_mincontainer(const uint32_t *weights, uint64_t *cumuls, const size_t size);

//DTE and DTD for unsigned 8-bit integer arrays encoded in unsigned 16-bit integer arrays
extern int encode_uint8_uint16_arbitrary(const uint8_t *in_array, uint16_t *out_array,
	const size_t size, const uint8_t min, const uint8_t max, const uint64_t *cumuls);
extern int decode_uint8_uint16_arbitrary(const uint16_t *in_array, uint8_t *out_array,
	const size_t size, const uint8_t min, const uint8_t max, const uint64_t *cumuls);

#endif
