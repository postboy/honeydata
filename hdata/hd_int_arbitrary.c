/*
components for arbitrary distributed integers
license: BSD 2-Clause
*/

#include "hd_int_arbitrary.h"

//convert weights to cumulative weights
#define GET_CUMULS \
do { \
	/*get size of weights and cumuls arrays*/ \
	wsize = max - min + 1; \
	\
	if ( (cumuls = malloc(wsize*sizeof(uint64_t))) == NULL ) { \
		error("couldn't allocate memory for cumuls"); \
		return -1; \
		} \
	\
	/*convert weights to cumulative weights: cumuls[i] = sum of weights[j], where j = 0..i*/ \
	current = 0; \
	for (i = 0; i < wsize; i++) { \
		prev = current; \
		current += weights[i]; \
		if (current < prev) { \
			error("integer overflow during cumuls computation"); \
			free(cumuls); \
			return -1; \
			} \
		cumuls[i] = current; \
		} \
} while(0)

#define ENCODE_IN_TYPE_ARBITRARY(ctype_t, ctype, otype) \
{ \
	/*index and weight of current element*/ \
	size_t index; \
	uint32_t weight; \
	/*cumulative weight of previous element*/ \
	uint64_t cumul_prev; \
	/*arrays for temporary and random data (actually this is the same array in memory)*/ \
	ctype_t *temp_array; \
	otype *rand_array; \
	/*return value*/ \
	int rv; \
	\
	if ( (temp_array = malloc(size*sizeof(otype))) == NULL ) { \
		error("couldn't allocate memory for temp_array"); \
		free(cumuls); \
		return -1; \
		} \
	randombytes( (unsigned char *)temp_array, size*sizeof(otype) ); \
	rand_array = (otype *)temp_array; \
	\
	if ( (*out_array = malloc(size*sizeof(otype))) == NULL ) { \
		error("couldn't allocate memory for out_array"); \
		free(cumuls); \
		free(temp_array); \
		return -1; \
		} \
	\
	for (i = 0; i < size; i++) { \
		if (in_array[i] < min) { \
			error("wrong min value"); \
			free(cumuls); \
			free(temp_array); \
			free(*out_array); \
			return -1; \
			} \
		else if (in_array[i] > max) { \
			error("wrong max value"); \
			free(cumuls); \
			free(temp_array); \
			free(*out_array); \
			return -1; \
			} \
		index = in_array[i] - min; \
		\
		if (index == 0) \
			cumul_prev = 0; \
		else \
			cumul_prev = cumuls[index-1]; \
		/*weight is the difference between two consecutive cumulative weights*/ \
		weight = cumuls[index] - cumul_prev; \
		\
		/*check if current value is impossible according to cumuls*/ \
		if (weight == 0) { \
			error("value in array is impossible according to cumuls"); \
			free(cumuls); \
			free(temp_array); \
			free(*out_array); \
			return -1; \
			} \
		\
		/*each temp value is pseudorandom value in [cumul_prev; cumuls[index]-1]*/ \
		/*we're very neat here: first we read rand_array member, which is two times bigger then \
		temp_array member, then we write temp_array member, possibly overwriting rand_array \
		member, which we don't need anymore*/ \
		temp_array[i] = (rand_array[i] % weight) + cumul_prev; \
		} \
	\
	/*finally encode uniformly distributed temporary values*/ \
	rv = encode_##ctype##_uniform(temp_array, (otype *)*out_array, size, 0, cumuls[wsize-1]); \
	\
	free(cumuls); \
	free(temp_array); \
	\
	/*if error happened, then return -1, else return size of output type in bytes*/ \
	if (rv) \
		return -1; \
	else \
		return sizeof(otype); \
}

extern int encode_uint8_arbitrary
(const uint8_t *in_array, void **out_array, const size_t size, const uint8_t min, \
const uint8_t max, const uint32_t *weights) { \
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
	if (weights == NULL) { \
		error("weights = NULL"); \
		return -1; \
		} \
	
	/*current and previous weights*/ \
	uint64_t current, prev; \
	/*cumulative weights*/ \
	uint64_t *cumuls; \
	size_t i, wsize; \
	
	GET_CUMULS;
	
	//check maximum value after encoding (i.e., maximum cumulative weight value)
	if (current < 256)				/*2^8*/
		ENCODE_IN_TYPE_ARBITRARY(uint8_t, uint8, uint16_t)
	else if (current < 65536)		/*2^16*/
		ENCODE_IN_TYPE_ARBITRARY(uint16_t, uint16, uint32_t)
	else if (current < 4294967296)	/*2^32*/
		ENCODE_IN_TYPE_ARBITRARY(uint32_t, uint32, uint64_t)
	else
		return -1;
}

#undef ENCODE_IN_TYPE_ARBITRARY

#define DECODE_IN_TYPE_ARBITRARY(ctype_t, ctype) \
{ \
	/*index and weight of current element*/ \
	size_t index; \
	/*uint32_t weight;*/ \
	/*temporary array*/ \
	ctype_t *temp_array; \
	/*return value*/ \
	int rv; \
	\
	if ( (temp_array = malloc(size*sizeof(ctype_t))) == NULL ) { \
		error("couldn't allocate memory for temp_array"); \
		free(cumuls); \
		return -1; \
		} \
	\
	/*firstly decode uniformly distributed temporary values*/ \
	/*optimization note: here we can use out_array instead of temp_array*/ \
	rv = decode_##ctype##_uniform(in_array, temp_array, size, 0, cumuls[wsize-1]); \
	if (rv < 0) { \
		free(cumuls); \
		free(temp_array); \
		return rv; \
		} \
	\
	for (i = 0; i < size; i++) { \
		/*this functionality can be optimized a lot!*/ \
		for (index = 0; index < (max - min + 1); index++) { \
			/*in this simple algorithm, (weight > 0) condition always holds, but normally we \
			should check this*/ \
			/*if (index == 0) \
				weight = cumuls[index]; \
			else \
				weight = cumuls[index] - cumuls[index-1];*/ \
			if ((temp_array[i] < cumuls[index]) /*&& (weight > 0)*/) \
				break; \
			} \
		\
		if (index == (max - min + 1)) { \
			error("can't find corresponding cumuls element"); \
			free(cumuls); \
			free(temp_array); \
			return -1; \
			} \
		\
		out_array[i] = index + min; \
		\
		/*if algorithm works right, this errors should never be thrown*/ \
		if (out_array[i] < min) { \
			error("algorithm error: wrong value < min"); \
			free(cumuls); \
			free(temp_array); \
			return -1; \
			} \
		else if (out_array[i] > max) { \
			error("algorithm error: wrong value > max"); \
			free(cumuls); \
			free(temp_array); \
			return -1; \
			} \
		} \
	\
	free(cumuls); \
	free(temp_array); \
	return 0; \
}

extern int decode_uint8_arbitrary
(const void *in_array, uint8_t *out_array, const size_t size, const uint8_t min, \
const uint8_t max, const uint32_t *weights) \
{ \
	/*check the arguments*/ \
	/*optimization note: some of this checks can safely be delegated to decode_uint8_uniform()*/ \
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
	if (weights == NULL) { \
		error("weights = NULL"); \
		return -1; \
		} \
	
	//current and previous weights
	uint64_t current, prev;
	//cumulative weights
	uint64_t *cumuls;
	size_t i, wsize;
	
	GET_CUMULS;
	
	//check maximum value after encoding (i.e., maximum cumulative weight value)
	if (current < 256)				/*2^8*/
		DECODE_IN_TYPE_ARBITRARY(uint8_t, uint8)
	else if (current < 65536)		/*2^16*/
		DECODE_IN_TYPE_ARBITRARY(uint16_t, uint16)
	else if (current < 4294967296)	/*2^32*/
		DECODE_IN_TYPE_ARBITRARY(uint32_t, uint32)
	else
		return -1;
}

#undef DECODE_IN_TYPE_ARBITRARY
