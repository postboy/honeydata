/*
components for arbitrary distributed integers
license: BSD 2-Clause
*/

#include "hd_int_arbitrary.h"

/*convert weights to cumulative weights and determine minimum size of container for values with
such cumulative weights*/
extern int get_cumuls_mincontainer(const uint32_t *weights, uint64_t *cumuls, const size_t size)
{
	//check the arguments
	if (weights == NULL) {
		error("weights = NULL");
		return -1;
		}
	if (cumuls == NULL) {
		error("cumuls = NULL");
		return -1;
		}
	if (size == 0) {
		error("size = 0");
		return -1;
		}
	
	//current and previous weights
	uint64_t current, prev;
	size_t i;
	
	//convert weights to cumulative weights: cumuls[i] = sum of weights[j], where j = 0..i
	current = 0;
	for (i = 0; i < size; i++) {
		prev = current;
		current += weights[i];
		if (current < prev) {
			error("integer overflow during cumuls computation");
			return -1;
			}
		cumuls[i] = current;
		}
	
	//check maximum value after encoding (i.e., maximum cumulative weight value)
	if (current < 256)				/*2^8*/
		return 8;
	else if (current < 65536)		/*2^16*/
		return 16;
	else if (current < 4294967296)	/*2^32*/
		return 32;
	else
		return 64;
}

extern int encode_uint8_uint16_arbitrary
(const uint8_t *in_array, uint16_t *out_array, const size_t size, const uint8_t min,
const uint8_t max, const uint64_t *cumuls) \
{ \
	/*check the arguments*/ \
	if (in_array == NULL) { \
		error("in_array = NULL"); \
		return -1; \
		} \
	if (out_array == NULL) { \
		error("out_array = NULL"); \
		return -1; \
		} \
	if (size == 0) { \
		error("size = 0"); \
		return -1; \
		} \
	if (min > max) { \
		error("min > max"); \
		return -1; \
		} \
	if (cumuls == NULL) { \
		error("cumuls = NULL"); \
		return -1; \
		} \
	
	/*index and weight of current element*/
	size_t index;
	uint32_t weight;
	/*temporary array*/
	uint8_t *temp_array;
	/*return value*/
	int rv;
	size_t i;
	
	if ( (temp_array = malloc(size*sizeof(uint8_t))) == NULL ) {
		error("couldn't allocate memory for temp_array");
		return -1;
		}
	
	for (i = 0; i < size; i++) {
		if (in_array[i] < min) { \
			error("wrong min value"); \
			free(temp_array); \
			return -1; \
			} \
		else if (in_array[i] > max) { \
			error("wrong max value"); \
			free(temp_array); \
			return -1; \
			} \
		index = in_array[i] - min;
		
		if (index == 0)
			weight = cumuls[index];
		else
			weight = cumuls[index] - cumuls[index-1];
		
		/*check if current value is impossible according to cumuls*/
		if (weight == 0) { \
			error("value in array is impossible according to cumuls"); \
			free(temp_array); \
			return -1; \
			} \
		
		temp_array[i] = cumuls[index];
		}
	
	/*finally encode uniformly distributed temporary values*/
	rv = encode_uint8_uniform(temp_array, out_array, size, 0, cumuls[max-min]);
	
	free(temp_array);
	return rv;
}

extern int decode_uint8_uint16_arbitrary
(const uint16_t *in_array, uint8_t *out_array, const size_t size, const uint8_t min, \
const uint8_t max, const uint64_t *cumuls) \
{ \
	/*check the arguments*/ \
	if (in_array == NULL) { \
		error("in_array = NULL"); \
		return -1; \
		} \
	if (out_array == NULL) { \
		error("out_array = NULL"); \
		return -1; \
		} \
	if (size == 0) { \
		error("size = 0"); \
		return -1; \
		} \
	if (min > max) { \
		error("min > max"); \
		return -1; \
		} \
	if (cumuls == NULL) { \
		error("cumuls = NULL"); \
		return -1; \
		} \
	
	/*index and weight of current element*/
	size_t index;
	/*uint32_t weight;*/
	/*temporary array*/
	uint8_t *temp_array;
	/*return value*/
	int rv;
	size_t i;
	
	if ( (temp_array = malloc(size*sizeof(uint8_t))) == NULL ) {
		error("couldn't allocate memory for temp_array");
		return -1;
		}
	
	/*firstly decode uniformly distributed temporary values*/
	/*optimization note: here we can use out_array instead of temp_array*/
	rv = decode_uint8_uniform(in_array, temp_array, size, min, max);
	if (rv < 0) {
		free(temp_array);
		return rv;
		}
	
	for (i = 0; i < size; i++) {
		/*this functionality can be optimized a lot!*/
		for (index = 0; index < (max - min + 1); index++) {
			/*if (index == 0)
				weight = cumuls[index];
			else
				weight = cumuls[index] - cumuls[index-1];*/
			/*in this simple algorithm weight > 0 always holds, but normally we should check this*/
			if ((temp_array[i] <= cumuls[index]) /*&& (weight > 0)*/)
				break;
			}
		
		if (index == (max - min + 1)) {
			error("can't find corresponding cumuls element"); \
			return -1; \
			}
		
		out_array[i] = index + min;
		
		/*if algorithm works right, this errors should never be thrown*/ \
		if (out_array[i] < min) { \
			error("algorithm error: wrong value < min"); \
			return -1; \
			} \
		else if (out_array[i] > max) { \
			error("algorithm error: wrong value > max"); \
			return -1; \
			} \
		}
	
	free(temp_array);
	return 0;
}
