/*
components for arbitrary distributed integers
license: BSD 2-Clause
*/

#include "hd_int_arbitrary.h"

//first check if we can handle such array, then convert weights to cumulative weights
/*optimization note: cumulative weights can be saved after computation in encoding stage and
just loaded (rather then re-computed) in decoding stage*/
#define CHECK_ARRAYS_GET_CUMULS() \
do { \
	/*get size of weights and cumuls arrays*/ \
	wsize_check = max - min + 1; \
	wsize = wsize_check; \
	\
	/*make a check for an overflow. it occurs if user wants to use too big weights and cumuls \
	arrays (i.e., size_t type can't hold their size). to catch this situation, we make \
	computations in biggest type available, then convert it to size_t. however, overflow can \
	occur even in biggest type, so we should catch for this situation too.*/ \
	/*optimization note: we can skip such check(s) if overflow(s) in question aren't possible, \
	for example when we're handling (u)int8_t arrays*/ \
	if ( (wsize != wsize_check) || ((max == UINT64_MAX) && (min == 0)) ) { \
		error("can't handle such big supplementary arrays"); \
		return -1; \
	} \
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
do { \
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
			*out_array = NULL; \
			return -1; \
			} \
		else if (in_array[i] > max) { \
			error("wrong max value"); \
			free(cumuls); \
			free(temp_array); \
			free(*out_array); \
			*out_array = NULL; \
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
			*out_array = NULL; \
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
	if (rv) { \
		free(*out_array); \
		*out_array = NULL; \
		return -1; \
		} \
	else { \
		return sizeof(otype); \
		} \
} while(0)

#define ENCODE_ARBITRARY(itype) \
(const itype *in_array, void **out_array, const size_t size, \
const itype min, const itype max, const uint32_t *weights) \
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
	if (weights == NULL) { \
		error("weights = NULL"); \
		return -1; \
		} \
	\
	/*current and previous weights*/ \
	uint64_t current, prev; \
	/*cumulative weights*/ \
	uint64_t *cumuls; \
	size_t i, wsize; \
	uint64_t wsize_check; \
	\
	CHECK_ARRAYS_GET_CUMULS(); \
	\
	/*check maximum value after encoding (i.e., maximum cumulative weight value)*/ \
	if (current < 256)				/*2^8*/ \
		ENCODE_IN_TYPE_ARBITRARY(uint8_t, uint8, uint16_t); \
	else if (current < 65536)		/*2^16*/ \
		ENCODE_IN_TYPE_ARBITRARY(uint16_t, uint16, uint32_t); \
	else if (current < 4294967296)	/*2^32*/ \
		ENCODE_IN_TYPE_ARBITRARY(uint32_t, uint32, uint64_t); \
	else { \
		error("too many values for any supported output type"); \
		free(cumuls); \
		return -1; \
		} \
}

extern int encode_uint8_arbitrary
	ENCODE_ARBITRARY(uint8_t)

extern int encode_int8_arbitrary
	ENCODE_ARBITRARY(int8_t)

extern int encode_uint16_arbitrary
	ENCODE_ARBITRARY(uint16_t)

extern int encode_int16_arbitrary
	ENCODE_ARBITRARY(int16_t)

extern int encode_uint32_arbitrary
	ENCODE_ARBITRARY(uint32_t)

extern int encode_int32_arbitrary
	ENCODE_ARBITRARY(int32_t)

extern int encode_uint64_arbitrary
	ENCODE_ARBITRARY(uint64_t)

extern int encode_int64_arbitrary
	ENCODE_ARBITRARY(int64_t)

#undef ENCODE_ARBITRARY
#undef ENCODE_IN_TYPE_ARBITRARY

#define DECODE_IN_TYPE_ARBITRARY(ctype_t, ctype) \
do { \
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
		/*optimization note: binary search or some other clever algorithm would be much faster*/ \
		for (index = 0; index < (max - min + 1); index++) { \
			/*in this simple algorithm, (weight > 0) condition always holds, but generally we \
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
} while (0)

#define DECODE_ARBITRARY(itype) \
(const void *in_array, itype *out_array, const size_t size, const itype min, \
const itype max, const uint32_t *weights) \
{ \
	/*check the arguments*/ \
	/*optimization note: some of this checks can safely be delegated to decode_ctype_uniform()*/ \
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
	\
	/*current and previous weights*/ \
	uint64_t current, prev; \
	/*cumulative weights*/ \
	uint64_t *cumuls; \
	size_t i, wsize; \
	uint64_t wsize_check; \
	\
	CHECK_ARRAYS_GET_CUMULS(); \
	\
	/*check maximum value after encoding (i.e., maximum cumulative weight value)*/ \
	if (current < 256)				/*2^8*/ \
		DECODE_IN_TYPE_ARBITRARY(uint8_t, uint8); \
	else if (current < 65536)		/*2^16*/ \
		DECODE_IN_TYPE_ARBITRARY(uint16_t, uint16); \
	else if (current < 4294967296)	/*2^32*/ \
		DECODE_IN_TYPE_ARBITRARY(uint32_t, uint32); \
	else { \
		error("too many values for any supported output type"); \
		free(cumuls); \
		return -1; \
		} \
}

extern int decode_uint8_arbitrary
	DECODE_ARBITRARY(uint8_t)

extern int decode_int8_arbitrary
	DECODE_ARBITRARY(int8_t)

extern int decode_uint16_arbitrary
	DECODE_ARBITRARY(uint16_t)

extern int decode_int16_arbitrary
	DECODE_ARBITRARY(int16_t)

extern int decode_uint32_arbitrary
	DECODE_ARBITRARY(uint32_t)

extern int decode_int32_arbitrary
	DECODE_ARBITRARY(int32_t)

extern int decode_uint64_arbitrary
	DECODE_ARBITRARY(uint64_t)

extern int decode_int64_arbitrary
	DECODE_ARBITRARY(int64_t)

#undef DECODE_ARBITRARY
#undef DECODE_IN_TYPE_ARBITRARY
#undef CHECK_ARRAYS_GET_CUMULS
