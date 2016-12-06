/*
components for floating-point numbers distributed uniformly
license: BSD 2-Clause
*/

#include "hd_fp_uniform.h"

//parameters in following generic functions:
//itype - type of input elements (floating-point type)
//otype - type of output elements (integer type of the same size as itype)
//MIDDLE - middle value of otype, i.e. first value of its code space's second half
//MASK - bitmask for setting is_quiet bit of NaNs

//generic function for converting fp arrays to integer arrays--------------------------------------

/*convert fp number to integer and back such way that each integer value will contain a number of
corresponding fp number in sequence of all possible fp numbers of this type. it means that negative
fp number with highest possible exponent and fraction will become 0 integer, and positive fp number
with highest possible exponent and fraction will become maximum value for corresponding integer
type.

is_quiet bit of all NaNs in array is assigned to random value during encoding, and is assigned to
1 during decoding.

optimization note: turns out that just running UINT_TO_FP without explicit setting of is_quite bit
may be enough to convert all signaling NaNs to quiet NaNs, but it may be not portable.*/

#define FP_TO_UINT(itype, otype, MIDDLE, MASK) \
(const itype *in_array, otype *out_array, const size_t size) \
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
	\
	/*current processing element before conversion. we use union notation here for access to \
	bitwise and integer arithmetic operations.*/ \
	union { \
		itype fp; \
		otype i; \
		} ielt; \
	otype oelt;	/*current processing element after conversion*/ \
	size_t i; \
	\
	/*write a random numbers to output array*/ \
	if (!RAND_bytes( (unsigned char *)out_array, size*sizeof(itype) )) { \
		ERR_print_errors_fp(stderr); \
		return 4; \
		} \
	\
	for (i = 0; i < size; i++) { \
		ielt.fp = in_array[i];		/*read the current element*/ \
		\
		/*if we process NaN then assign random value to its is_quiet bit*/ \
		if (isnan(ielt.fp)) { \
			/*if random value is in first half of code space then set bit, else clear bit*/ \
			if ( out_array[i] < (MIDDLE) ) \
				ielt.i = ielt.i | (MASK); \
			else \
				ielt.i = ielt.i & ~(MASK); \
			} \
		\
		if (ielt.i < MIDDLE)		/*if it has a positive value as fp number*/ \
			oelt = ielt.i + (MIDDLE); \
		else \
			oelt = ~ielt.i; \
		\
		out_array[i] = oelt; \
		} \
	\
	return 0; \
}

extern int float_to_uint32
	FP_TO_UINT(float, uint32_t, 0x80000000, 0x00400000)

extern int double_to_uint64
	FP_TO_UINT(double, uint64_t, 0x8000000000000000, 0x0008000000000000)

#undef FP_TO_UINT

//generic function for converting integer arrays back to fp arrays---------------------------------

#define UINT_TO_FP(itype, otype, MIDDLE, MASK) \
(const otype *in_array, itype *out_array, const size_t size) \
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
	\
	otype oelt;	/*current processing element before conversion*/ \
	/*current processing element after conversion. we use union notation here for access to \
	bitwise and integer arithmetic operations.*/ \
	union { \
		itype fp; \
		otype i; \
		} ielt; \
	size_t i; \
	\
	for (i = 0; i < size; i++) { \
		oelt = in_array[i];			/*read the current element*/ \
		\
		if (oelt >= (MIDDLE) ) 		/*if it has a positive value as fp number*/ \
			ielt.i = oelt - (MIDDLE); \
		else \
			ielt.i = ~oelt; \
		\
		/*if we process NaN then assign 1 to its is_quiet bit*/ \
		if (isnan(ielt.fp)) \
			ielt.i = ielt.i | (MASK); \
		\
		out_array[i] = ielt.fp; \
		} \
	\
	return 0; \
}

extern int uint32_to_float
	UINT_TO_FP(float, uint32_t, 0x80000000, 0x00400000)
extern int uint64_to_double
	UINT_TO_FP(double, uint64_t, 0x8000000000000000, 0x0008000000000000)

#undef UINT_TO_FP

extern int container_float_uniform(const float min, const float max)
{
	/*check the arguments*/
	if (min > max) {
		error("min > max");
		return 4;
		}
	
	/*components of minimum and maximum: sign, exponent and fraction*/
	bool minsign, maxsign;		/*true - positive number, false - negative*/
	uint16_t minexp, maxexp;
	uint32_t minfrac, maxfrac;
	
	/*number of used groups, weight of current group, number of additional
	elements*/
	uint64_t used_groups, weight, remainder, i;
	
	/*we use this union for logical operations on fp numbers*/
	union {
		float fl;
		uint32_t i;
		} hfloat;
	
	/*get components of minimum and maximum*/
	hfloat.fl = min;
	minsign = (hfloat.i >> 31 ) ? false : true;
	minexp = (hfloat.i >> 23) & 0xFF;
	minfrac = hfloat.i & 0x007FFFFF;
	
	hfloat.fl = max;
	maxsign = (hfloat.i >> 31 ) ? false : true;
	maxexp = (hfloat.i >> 23) & 0xFF;
	maxfrac = hfloat.i & 0x007FFFFF;
	
	/*if both minimum and maximum have same sign*/
	if (minsign == maxsign) {
		/*if both of them are negative then swap their components*/
		if (!maxsign) {
			uint16_t tmpexp;
			uint32_t tmpfrac;
			
			tmpexp = minexp;
			minexp = maxexp;
			maxexp = tmpexp;
			
			tmpfrac = minfrac;
			minfrac = maxfrac;
			maxfrac = tmpfrac;
			}
		
		/*count number of fully used groups*/
		used_groups = 0;
		weight = 2;
		/*this fragment can be greatly optimized using fact that
		(2^0 + ... + 2^n) = (2^(n+1) - 1)*/
		for (i = minexp + 1; i < maxexp; i++) {
			used_groups += weight;
			weight <<= 1;
			}
		
		/*count all additional elements*/
		if (minexp == maxexp)
			remainder = maxfrac - minfrac;
		else {
			/*this actions can be optimized by converting operations from
			arithmetical to logical*/
			/*first part of remainder is contained in maxfrac*/
			remainder = maxfrac*weight;
			/*maybe there is some more groups in remainder now?*/
			used_groups += remainder >> 23;
			remainder &= 0x007FFFFF;
			/*second part of remainder is contained in minfrac*/
			remainder += 0x007FFFFF - minfrac;
			/*maybe there is some more groups in remainder now?*/
			used_groups += remainder >> 23;
			remainder &= 0x007FFFFF;
			}
		
		/*if we need to use one more group then use it*/
		if (remainder != 0)
			used_groups++;		
		}
	/*if minimum is negative and maximum is positive*/
	else {
		/*smallest_exponent = 1;*/
		}
	
	if (used_groups < 512)					/*2^9*/
		return UINT32;
	else if (used_groups < 2199023255552)	/*2^41*/
		return UINT64;
	else
		return TOO_LONG;
	
	return 0;
}
