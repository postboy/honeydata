/*
components for integers distributed uniformly
license: BSD 2-Clause
*/

#include "hd_int_uniform.h"

//parameters in following generic functions:
//itype - type of input elements
//utype - unsigned type of same size as input type
//otype - type of output elements (always unsigned, size is twice larger than size of itype)
//UTYPE_MAX, OTYPE_MAX - maximum possible values of utype and otype, respectively
//ISPACE - size of itype and utype code space (equals UTYPE_MAX + 1)
//OSPACE - size of otype code space (equals OTYPE_MAX + 1)

//generic DTE function for encoding integer arrays in integer arrays-------------------------------

#define ENCODE_IN_INT_UNIFORM(itype, utype, otype, UTYPE_MAX, OTYPE_MAX) \
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
	/*current processing element after type promotion*/ \
	otype oelt; \
	/*size of full group in elements, from 1 to (itype_MAX-itype_MIN+1)*/ \
	const otype group_size = (otype)max - min + 1; \
	/*number of elements in the last group or 0 if the last group is full, from 0 to ISPACE-1. \
	original formula was OSPACE % group_size, but it didn't work for uint64_t: \
	UINT64_MAX + 1 = 0 because of integer overflow. modular arithmetic used here: \
	(a + b) mod c = ( (a mod c) + (b mod c) ) mod c, but since c >= 2 where last_group_size is \
	used, then 1 mod c = 1.*/\
	const utype last_group_size = ( (OTYPE_MAX) % group_size + 1 ) % group_size; \
	size_t i; \
	\
	/*if every value is possible*/ \
	/*note the type promotion here: (max_type_number+1) can become 0 without it because of \
	integer overflow!*/ \
	if (group_size == ( (otype)(UTYPE_MAX)+1 ) ) { \
		/*then just copy input array to output array to create a first part*/ \
		memcpy(out_array, in_array, size*sizeof(itype)); \
		/*follow it by random numbers to create a second part*/ \
		randombytes( (unsigned char *)out_array + size*sizeof(itype), \
				size*(sizeof(otype) - sizeof(itype)) ); \
		return 0; \
		} \
	\
	/*write the random numbers to output array*/ \
	randombytes( (unsigned char *)out_array, size*sizeof(otype) ); \
	\
	/*if only one value is possible then use a random number for encoding each number*/ \
	if (group_size == 1) { \
		/*we're already done with random numbers, but we should check an input array*/ \
		for (i = 0; i < size; i++) { \
			if (in_array[i] != min) { \
				error("wrong min or max value"); \
				return 5; \
				} \
			} \
		return 0; \
		} \
	\
	/*total number of groups (from ISPACE+1 to OSPACE/2), so they will have indexes in interval \
	[0; group_num-1]. original formula was ceill( (long double)OSPACE / group_size), but this \
	formula is faster, more portable and reliable. see math.c for equivalence proof.*/ \
	const otype group_num = (OTYPE_MAX) / group_size + 1; \
	\
	/*else encode each number using random numbers from out_array for group selection*/ \
	for (i = 0; i < size; i++) { \
		if (in_array[i] < min) { \
			error("wrong min value"); \
			return 6; \
			} \
		else if (in_array[i] > max) { \
			error("wrong max value"); \
			return 7; \
			} \
		/*note type promotion here: algorithm don't work right without it on e.g. int32 tests*/ \
		oelt = in_array[i] - (otype)min; 		/*normalize current element and make type promotion*/ \
		\
		/*if we can place the current element in any group (including the last one) then do it*/ \
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

extern int encode_uint8_uniform
	ENCODE_IN_INT_UNIFORM(uint8_t, uint8_t, uint16_t, UINT8_MAX, UINT16_MAX)

extern int encode_int8_uniform
	ENCODE_IN_INT_UNIFORM(int8_t, uint8_t, uint16_t, UINT8_MAX, UINT16_MAX)

extern int encode_uint16_uniform
	ENCODE_IN_INT_UNIFORM(uint16_t, uint16_t, uint32_t, UINT16_MAX, UINT32_MAX)

extern int encode_int16_uniform
	ENCODE_IN_INT_UNIFORM(int16_t, uint16_t, uint32_t, UINT16_MAX, UINT32_MAX)

extern int encode_uint32_uniform
	ENCODE_IN_INT_UNIFORM(uint32_t, uint32_t, uint64_t, UINT32_MAX, UINT64_MAX)

extern int encode_int32_uniform
	ENCODE_IN_INT_UNIFORM(int32_t, uint32_t, uint64_t, UINT32_MAX, UINT64_MAX)

#undef ENCODE_IN_INT_UNIFORM

//generic DTE function for encoding integer arrays in mpz_t arrays---------------------------------

#define ENCODE_IN_MPZ_UNIFORM(itype) \
(const itype *in_array, mpz_t *out_array, const size_t size, const itype min, const itype max) \
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
	/*current processing element after type promotion*/ \
	mpz_t oelt; \
	/*size of full group in elements, from 1 to (itype_MAX-itype_MIN+1)*/ \
	mpz_t group_size; \
	/*total number of groups (from ISPACE+1 to OSPACE/2), so they will have indexes in interval \
	[0; group_num-1]. original formula was	ceil(OSPACE / group_size), but this formula is \
	faster, more portable and reliable. see math.c for equivalence proof for smaller types.*/ \
	mpz_t group_num; \
	/*temporary variable for different computations*/ \
	mpz_t tmp; \
	/*number of elements in the last group or 0	if the last group is full, from	0 to ISPACE-1.*/ \
	uint64_t last_group_size; \
	/*normalized value of current element*/ \
	uint64_t normalized; \
	/*store for random bytes*/ \
	unsigned char *rand_data; \
	size_t i; \
	\
	/*write the random numbers to temporary array*/ \
	if ( (rand_data = malloc(size*16)) == NULL ) { \
		error("couldn't allocate memory for rand_data"); \
		return 5; \
		} \
	randombytes(rand_data, size*16); \
	\
	/*if only one value is possible then use a random number for encoding each number*/ \
	if (min == max) { \
		/*first, we should check an input array*/ \
		for (i = 0; i < size; i++) { \
			if (in_array[i] != min) { \
				error("wrong min or max value"); \
				return 6; \
				} \
			} \
		/*copy random numbers to output array*/ \
		for (i = 0; i < size; i++) \
			/*maybe parameters of such calls can be optimized for simplification of \
			mpz_import()'s job. we can't use mpz_urandomb() here because without subtle seed \
			handling it won't produce cryptographically secure results.*/ \
			mpz_import(out_array[i], 16/sizeof(int), 1, sizeof(int), 0, 0, rand_data+i*16); \
		\
		free(rand_data); \
		return 0; \
		} \
	\
	mpz_inits(oelt, group_size, group_num, tmp, NULL); \
	\
	/*group_size = max - min + 1*/ \
	/*load second half, then first half: oelt = second_half << 32 + first_half. we do this \
	because long int type can have size of 4 bytes, while our itype has size of 8 bytes.*/ \
	mpz_set_ui(group_size, max >> 32); \
	mpz_mul_2exp(group_size, group_size, 32); \
	mpz_add_ui(group_size, group_size, max & 0xFFFFFFFF); \
	mpz_set_ui(tmp, min >> 32); \
	mpz_mul_2exp(tmp, tmp, 32); \
	mpz_add_ui(tmp, tmp, min & 0xFFFFFFFF); \
	mpz_sub(group_size, group_size, tmp); \
	mpz_add_ui(group_size, group_size, 1); \
	\
	/*last_group_size = OSPACE % group_size, where OSPACE = ISPACE^2, ISPACE = UINT64_MAX + 1 = \
	= (UINT32_MAX + 1)^2, then last_group_size = (UINT32_MAX + 1)^4 % group_size*/ \
	mpz_set_ui(tmp, UINT32_MAX); \
	mpz_add_ui(tmp, tmp, 1); \
	mpz_pow_ui(tmp, tmp, 4); \
	/*save this intermediate result for group_num computation*/ \
	mpz_set(group_num, tmp); \
	/*note: mpz_tdiv and mpz_fdiv function families will return the same results in	this \
	algorithm, since n >= 0. it means that we can use fdiv here, if it will be beneficial for \
	some reason. however, usage of tdiv should be more obvious for reader.*/ \
	mpz_tdiv_q(tmp, tmp, group_size); \
	last_group_size = mpz_get_ui(tmp); \
	\
	/*group_num = OTYPE_MAX / group_size + 1, where OTYPE_MAX = OSPACE - 1 = \
	= (UINT32_MAX + 1)^4 - 1*/ \
	mpz_sub_ui(group_num, group_num, 1); \
	mpz_tdiv_q(group_num, group_num, group_size); \
	/*save this intermediate result for future computations*/ \
	mpz_set(tmp, group_num); \
	mpz_add_ui(group_num, group_num, 1); \
	\
	/*else encode each number using random numbers from out_array for group selection*/ \
	for (i = 0; i < size; i++) { \
		if (in_array[i] < min) { \
			error("wrong min value"); \
			return 7; \
			} \
		else if (in_array[i] > max) { \
			error("wrong max value"); \
			return 8; \
			} \
		\
		/*normalize current element and make type promotion: oelt = in_array[i] - min*/ \
		normalized = in_array[i] - min; \
		mpz_set_ui(oelt, normalized >> 32); \
		mpz_mul_2exp(oelt, oelt, 32); \
		mpz_add_ui(oelt, oelt, normalized & 0xFFFFFFFF); \
		\
		/*if we can place the current element in any group (including the last one) then do it*/ \
		if ( (normalized < last_group_size) || (last_group_size == 0) ) { \
			/*oelt += (out_array[i] % group_num) * group_size*/ \
			mpz_import(out_array[i], 16/sizeof(int), 1, sizeof(int), 0, 0, rand_data+i*16); \
			mpz_tdiv_q(out_array[i], out_array[i], group_num); \
			mpz_mul(out_array[i], out_array[i], group_size); \
			mpz_add(out_array[i], out_array[i], oelt); \
			} \
		/*else place it in any group excluding the last one*/ \
		else { \
			/*oelt += ( out_array[i] % (group_num-1) ) * group_size*/ \
			mpz_import(out_array[i], 16/sizeof(int), 1, sizeof(int), 0, 0, rand_data+i*16); \
			mpz_tdiv_q(out_array[i], out_array[i], tmp); \
			mpz_mul(out_array[i], out_array[i], group_size); \
			mpz_add(out_array[i], out_array[i], oelt); \
			} \
		} \
	\
	free(rand_data); \
	mpz_clears(oelt, group_size, group_num, tmp, NULL); \
	return 0; \
}

extern int encode_uint64_uniform
	ENCODE_IN_MPZ_UNIFORM(uint64_t)

extern int encode_int64_uniform
	ENCODE_IN_MPZ_UNIFORM(int64_t)

#undef ENCODE_IN_MPZ_UNIFORM

//generic DTE function for extracting integer arrays from integer arrays---------------------------

#define DECODE_IN_INT_UNIFORM(itype, otype, UTYPE_MAX) \
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
	/*size of full group in elements, from 1 to (itype_MAX-itype_MIN+1)*/ \
	const otype group_size = (otype)max - min + 1; \
	size_t i; \
	\
	/*if every value is possible then just copy first part of input array to output array*/ \
	/*note the type promotion here: (max_type_number+1) can become 0 without it because of \
	integer overflow!*/ \
	if (group_size == ( (otype)(UTYPE_MAX)+1 ) ) { \
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
		/*get its value in first group, denormalize it, do a type regression*/ \
		out_array[i] = (in_array[i] % group_size) + min; \
		\
		/*if algorithm works right, this errors should never be thrown*/ \
		if (out_array[i] < min) { \
			error("algorithm error: wrong value < min"); \
			return 5; \
			} \
		else if (out_array[i] > max) { \
			error("algorithm error: wrong value > max"); \
			return 6; \
			} \
		} \
	\
	return 0; \
}

extern int decode_uint8_uniform
	DECODE_IN_INT_UNIFORM(uint8_t, uint16_t, UINT8_MAX)

extern int decode_int8_uniform
	DECODE_IN_INT_UNIFORM(int8_t, uint16_t, UINT8_MAX)

extern int decode_uint16_uniform
	DECODE_IN_INT_UNIFORM(uint16_t, uint32_t, UINT16_MAX)

extern int decode_int16_uniform
	DECODE_IN_INT_UNIFORM(int16_t, uint32_t, UINT16_MAX)

extern int decode_uint32_uniform
	DECODE_IN_INT_UNIFORM(uint32_t, uint64_t, UINT32_MAX)

extern int decode_int32_uniform
	DECODE_IN_INT_UNIFORM(int32_t, uint64_t, UINT32_MAX)

#undef DECODE_IN_INT_UNIFORM

//generic DTE function for extracting integer arrays from mpz_t arrays-----------------------------

#define DECODE_IN_MPZ_UNIFORM(itype) \
(const mpz_t *in_array, itype *out_array, const size_t size, const itype min, const itype max) \
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
	/*current processing element before type promotion*/ \
	mpz_t ielt; \
	/*size of full group in elements, from 1 to (itype_MAX-itype_MIN+1)*/ \
	mpz_t group_size; \
	/*temporary variable for different computations*/ \
	mpz_t tmp; \
	/*normalized value of current element*/ \
	uint64_t normalized; \
	size_t i; \
	\
	/*if only one value is possible then fill output array with this value*/ \
	if (min == max) { \
		for (i = 0; i < size; i++) \
			out_array[i] = min; \
		return 0; \
		} \
	\
	mpz_inits(ielt, group_size, tmp, NULL); \
	\
	/*group_size = max - min + 1*/ \
	mpz_set_ui(group_size, max >> 32); \
	mpz_mul_2exp(group_size, group_size, 32); \
	mpz_add_ui(group_size, group_size, max & 0xFFFFFFFF); \
	mpz_set_ui(tmp, min >> 32); \
	mpz_mul_2exp(tmp, tmp, 32); \
	mpz_add_ui(tmp, tmp, min & 0xFFFFFFFF); \
	mpz_sub(group_size, group_size, tmp); \
	mpz_add_ui(group_size, group_size, 1); \
	\
	/*else decode each number*/ \
	for (i = 0; i < size; i++) { \
		/*get its value in first group, denormalize it, do a type regression*/ \
		/*out_array[i] = (in_array[i] % group_size) + min*/ \
		mpz_set(ielt, in_array[i]); \
		mpz_tdiv_r(ielt, ielt, group_size); \
		/*save second half (i.e. its most significant 4 bytes) of ielt in tmp, first half (i.e. \
		its least significant 4 bytes) in ielt*/ \
		mpz_tdiv_q_2exp(tmp, ielt, 32); \
		mpz_tdiv_r_2exp(ielt, ielt, 32); \
		\
		normalized = mpz_get_ui(tmp); \
		normalized <<= 32; \
		normalized += mpz_get_ui(ielt); \
		out_array[i] = normalized + min; \
		\
		/*if algorithm works right, this errors should never be thrown*/ \
		if (out_array[i] < min) { \
			error("algorithm error: wrong value < min"); \
			return 5; \
			} \
		else if (out_array[i] > max) { \
			error("algorithm error: wrong value > max"); \
			return 6; \
			} \
		} \
	\
	mpz_clears(ielt, group_size, tmp, NULL); \
	return 0; \
}

extern int decode_uint64_uniform
	DECODE_IN_MPZ_UNIFORM(uint64_t)

extern int decode_int64_uniform
	DECODE_IN_MPZ_UNIFORM(int64_t)

#undef DECODE_IN_MPZ_UNIFORM
