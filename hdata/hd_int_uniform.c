/*
hd_int_uniform.c - components for integers distributed uniformly
License: BSD 2-Clause
*/

#include "hd_int_uniform.h"

//parameters in following generic functions:
//itype - type of input elements
//utype - unsigned type of same size as input type
//otype - type of output elements (always unsigned, size is twice larger than size of itype)
//ISPACE - size of utype code space
//OSPACE - size of otype code space

//generic function for finding minimum and maximum in array----------------------------------------
#define GET_ARRAY_MINMAX(itype) \
(const itype *array, const size_t size, itype *min, itype *max) \
{ \
	\
	/*check the arguments*/ \
	if (array == NULL) { \
		error("array = NULL"); \
		return 1; \
		} \
	if (size == 0) { \
		error("size = 0"); \
		return 2; \
		} \
	if (min == NULL) { \
		error("min = NULL"); \
		return 3; \
		} \
	if (max == NULL) { \
		error("max = NULL"); \
		return 4; \
		} \
	\
	itype tmpmin, tmpmax;	/*variables for storing temporary minimum and maximum values*/ \
	size_t i; \
	\
	/*initialize minimum and maximum values*/ \
	tmpmin = array[0]; \
	tmpmax = array[0]; \
	/*let's try to find smaller minimum and bigger maximum*/ \
	for (i = 1; i < size; i++) { \
		if (array[i] < tmpmin)		/*then it's the new minimum*/ \
			tmpmin = array[i]; \
		if (array[i] > tmpmax)		/*then it's the new maximum*/ \
			tmpmax = array[i]; \
		/*we don't break a cycle if ((tmpmin == itype_MIN) && (tmpmax == itype_MAX)) because it \
		can be used for timing attack*/ \
		} \
	\
	/*finally copy results to output buffers*/ \
	*min = tmpmin; \
	*max = tmpmax; \
	\
	return 0; \
}

//it's concretizations
extern int8_t get_uint8_minmax
	GET_ARRAY_MINMAX(uint8_t)
extern int8_t get_int8_minmax
	GET_ARRAY_MINMAX(int8_t)
extern int8_t get_uint16_minmax
	GET_ARRAY_MINMAX(uint16_t)
extern int8_t get_int16_minmax
	GET_ARRAY_MINMAX(int16_t)
extern int8_t get_uint32_minmax
	GET_ARRAY_MINMAX(uint32_t)
extern int8_t get_int32_minmax
	GET_ARRAY_MINMAX(int32_t)

#undef GET_ARRAY_MINMAX

//generic DTE function for integer arrays----------------------------------------------------------
#define ENCODE_INT_UNIFORM(itype, utype, otype, ISPACE, OSPACE) \
(const itype *in_array, otype *out_array, const size_t size, const itype min, const itype max) \
{ \
	/*check the arguments*/ \
	if (in_array == NULL) { \
		error("in_array = NULL"); \
		return 1; \
		} \
	if (out_array == NULL) { \
		error("out_array = NULL"); \
		return 2; \
		} \
	if (size == 0) { \
		error("size = 0"); \
		return 3; \
		} \
	if (min > max) { \
		error("min > max"); \
		return 4; \
		} \
	\
	size_t i; \
	itype ielt;	/*current processing element before and after type promotion*/ \
	otype oelt; \
	/*size of a full group in elements, from 1 to (itype_MAX-itype_MIN+1)*/ \
	const otype group_size = max - min + 1; \
	/*number of elements in the last group or 0 if the last group is full, from 0 to ISPACE-1*/ \
	const utype last_group_size = (OSPACE) % group_size; \
	\
	/*if every value is possible*/ \
	if (group_size == (ISPACE) ) { \
		/*then just copy input array to output array to create a first half*/ \
		memcpy(out_array, in_array, size*sizeof(itype)); \
		/*follow it by random numbers to create a second half*/ \
		if (!RAND_bytes( (unsigned char *)out_array + size*sizeof(itype), size*sizeof(itype) )) { \
    		ERR_print_errors_fp(stderr); \
    		return 5; \
    		} \
		return 0; \
		} \
	\
	/*write a random numbers to output array*/ \
	if (!RAND_bytes( (unsigned char *)out_array, 2*size*sizeof(itype) )) { \
    	ERR_print_errors_fp(stderr); \
    	return 6; \
    	} \
	\
	/*if only one value is possible then use a random number for encoding each number*/ \
	if (group_size == 1) { \
		/*we're already done with random numbers, but we should check an input array*/ \
		for (i = 0; i < size; i++) { \
			ielt = in_array[i]; \
			if (ielt < min) { \
				error("wrong min value"); \
				return 7; \
				} \
			else if (ielt > max) { \
				error("wrong max value"); \
				return 8; \
				} \
			} \
		return 0; \
		} \
	\
	/*number of groups (from ISPACE+1 to OSPACE/2), so they will have values in interval \
	[0; group_num-1]; notice type conversion here - we want float result of division instead of \
	integer!*/ \
	const otype group_num = ceil( (OSPACE) / (float)group_size); \
	\
	/*else encode each number using random numbers from out_array for group selection*/ \
	for (i = 0; i < size; i++) { \
		ielt = in_array[i]; \
		if (ielt < min) { \
			error("wrong min value"); \
			return 9; \
			} \
		else if (ielt > max) { \
			error("wrong max value"); \
			return 10; \
			} \
		oelt = ielt; 		/*make type promotion to begin encoding*/ \
		oelt = oelt - min;	/*normalize current element*/ \
		\
		/*if we can place a current element in any group (including the last one) then do it*/ \
		if ( (oelt < last_group_size) || (last_group_size == 0) ) \
			oelt += (out_array[i] % group_num) * group_size; \
		/*else place it in any group excluding the last one*/ \
		else \
			oelt += ( out_array[i] % (group_num-1) ) * group_size; \
		\
		out_array[i] = oelt;	/*finally write it to buffer*/ \
		} \
	\
	return 0; \
}

//note the type promotions here: (max_type_number+1) can become 0 without them!
extern int8_t encode_uint8_uniform
	ENCODE_INT_UNIFORM(uint8_t, uint8_t, uint16_t,
						( (uint16_t)UINT8_MAX+1 ), ( (uint32_t)UINT16_MAX+1 ) )
extern int8_t encode_int8_uniform
	ENCODE_INT_UNIFORM(int8_t, uint8_t, uint16_t,
						( (uint16_t)UINT8_MAX+1 ), ( (uint32_t)UINT16_MAX+1 ) )
extern int8_t encode_uint16_uniform
	ENCODE_INT_UNIFORM(uint16_t, uint16_t, uint32_t,
						( (uint32_t)UINT16_MAX+1 ), ( (uint64_t)UINT32_MAX+1 ) )
extern int8_t encode_int16_uniform
	ENCODE_INT_UNIFORM(int16_t, uint16_t, uint32_t,
						( (uint32_t)UINT16_MAX+1 ), ( (uint64_t)UINT32_MAX+1 ) )

#undef ENCODE_INT_UNIFORM

//generic DTD function for integer arrays----------------------------------------------------------
#define DECODE_INT_UNIFORM(itype, otype, ISPACE) \
(const otype *in_array, itype *out_array, const size_t size, const itype min, const itype max) \
{ \
	/*check the arguments*/ \
	if (in_array == NULL) { \
		error("in_array = NULL"); \
		return 1; \
		} \
	if (out_array == NULL) { \
		error("out_array = NULL"); \
		return 2; \
		} \
	if (size == 0) { \
		error("size = 0"); \
		return 3; \
		} \
	if (min > max) { \
		error("min > max"); \
		return 4; \
		} \
	\
	size_t i; \
	otype oelt; /*current processing element before and after type regression*/ \
	itype ielt;	\
	/*size of a full group in elements, from 1 to (itype_MAX-itype_MIN+1)*/ \
	const otype group_size = max - min + 1; \
	\
	/*if every value is possible then just copy first half of input array to output array*/ \
	if (group_size == (ISPACE) ) { \
		memcpy(out_array, in_array, size*sizeof(itype)); \
		return 0; \
		} \
	\
	/*if only one value is possible then fill output array with this value*/ \
	if (group_size == 1) { \
		for (i = 0; i < size; i++) \
			out_array[i] = min; \
		return 0; \
		} \
	\
	/*else decode each number*/ \
	for (i = 0; i < size; i++) { \
		oelt = in_array[i];	/*read current element*/ \
		/*get it's value in first group, denormalize it, do a type regression*/ \
		ielt = (oelt % group_size) + min; \
		\
		/*if algorithm works right, this errors should never been thrown*/ \
		if (ielt < min) { \
			error("algorithm error: wrong value < min"); \
			return 5; \
			} \
		else if (ielt > max) { \
			error("algorithm error: wrong value > max"); \
			return 6; \
			} \
		\
		out_array[i] = ielt;	/*finally write it to buffer*/ \
		} \
	\
	return 0; \
}

//note the type promotions here: (max_type_number+1) can become 0 without them!
extern int8_t decode_uint8_uniform
	DECODE_INT_UNIFORM(uint8_t, uint16_t, ( (uint16_t)UINT8_MAX+1 ) )
extern int8_t decode_int8_uniform
	DECODE_INT_UNIFORM(int8_t, uint16_t, ( (uint16_t)UINT8_MAX+1 ) )
extern int8_t decode_uint16_uniform
	DECODE_INT_UNIFORM(uint16_t, uint32_t, ( (uint32_t)UINT16_MAX+1 ) )
extern int8_t decode_int16_uniform
	DECODE_INT_UNIFORM(int16_t, uint32_t, ( (uint32_t)UINT16_MAX+1 ) )

#undef DECODE_INT_UNIFORM
