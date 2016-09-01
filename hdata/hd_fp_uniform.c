/*
components for floating-point numbers distributed uniformly
license: BSD 2-Clause
*/

#include "hd_int_uniform.h"

/*convert float to uint32 and back in such way that each uint32 value will contain a number of
corresponding float in sequence of all possible floats. it means that negative float with lowest
possible exponent and fraction will become 0 integer, and positive float with highest possible
exponent and fraction will become UINT32_MAX integer.*/
extern int8_t float_to_uint32(const float *in_array, uint32_t *out_array, const size_t size)
{
	/*check the arguments*/
	if (in_array == NULL) {
		error("in_array = NULL");
		return 1;
		}
	if (out_array == NULL) {
		error("out_array = NULL");
		return 2;
		}
	if (size == 0) {
		error("size = 0");
		return 3;
		}
	
	/*current processing element before conversion. we use union notation here for access to
	bitwise and integer arithmetic operations.*/
	union {
		float fp;
		uint32_t i;
		} ielt;
	uint32_t oelt;	/*current processing element after conversion*/
	size_t i;
	
	for (i = 0; i < size; i++) {
		ielt.fp = in_array[i];			/*read a current element*/
		
		/*if it's a positive value*/
		if (ielt.i < 0x80000000)
			oelt = ielt.i + 0x80000000;
		else
			oelt = 0xFFFFFFFF - ielt.i;
		
		out_array[i] = oelt;
		}

	return 0;
}
extern int8_t uint32_to_float(const float *in_array, uint32_t *out_array, const size_t size)
{
	/*check the arguments*/
	if (in_array == NULL) {
		error("in_array = NULL");
		return 1;
		}
	if (out_array == NULL) {
		error("out_array = NULL");
		return 2;
		}
	if (size == 0) {
		error("size = 0");
		return 3;
		}
	
	uint32_t ielt;	/*current processing element before conversion*/
	/*current processing element after conversion. we use union notation here for access to
	integer arithmetic operations.*/
	union {
		float fp;
		uint32_t i;
		} oelt;
	size_t i;
	
	for (i = 0; i < size; i++) {
		ielt = in_array[i];			/*read a current element*/
		
		/*if it's a positive value*/
		if (ielt >= 0x80000000)
			oelt.i = ielt - 0x80000000;
		else
			oelt.i = 0xFFFFFFFF - ielt;
		
		out_array[i] = oelt.fp;
		}

	return 0;
}
