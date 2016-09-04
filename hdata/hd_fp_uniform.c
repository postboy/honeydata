/*
components for floating-point numbers distributed uniformly
license: BSD 2-Clause
*/

#include "hd_int_uniform.h"

//parameters in following generic functions:
//itype - type of input elements (floating-point type)
//otype - type of output elements (integer type of the same size as itype)
//MIDDLE - middle value of otype, i.e. first value of its code space's second half
//END - maximum value of otype, i.e. last value of its code space

//generic function for converting fp arrays to integer arrays--------------------------------------

/*convert fp number to integer and back such way that each integer value will contain a number of
corresponding fp number in sequence of all possible fp numbers of this type. it means that negative
fp number with highest possible exponent and fraction will become 0 integer, and positive fp number
with highest possible exponent and fraction will become maximum value for corresponding integer
type.*/

#define FP_TO_UINT(itype, otype, MIDDLE, END) \
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
	integer arithmetic operations.*/ \
	union { \
		itype fp; \
		otype i; \
		} ielt; \
	otype oelt;	/*current processing element after conversion*/ \
	size_t i; \
	\
	for (i = 0; i < size; i++) { \
		ielt.fp = in_array[i];		/*read the current element*/ \
		\
		if (ielt.i < MIDDLE)		/*if it has a positive value as fp number*/ \
			oelt = ielt.i + MIDDLE; \
		else \
			oelt = END - ielt.i; \
		\
		out_array[i] = oelt; \
		} \
	\
	return 0; \
}

extern int8_t float_to_uint32
	FP_TO_UINT(float, uint32_t, 0x80000000, 0xFFFFFFFF)

extern int8_t double_to_uint64
	FP_TO_UINT(double, uint64_t, 0x8000000000000000, 0xFFFFFFFFFFFFFFFF)

#undef FP_TO_UINT

//generic function for converting integer arrays back to fp arrays---------------------------------

#define UINT_TO_FP(itype, otype, MIDDLE, END) \
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
	otype ielt;	/*current processing element before conversion*/ \
	/*current processing element after conversion. we use union notation here for access to \
	integer arithmetic operations.*/ \
	union { \
		itype fp; \
		otype i; \
		} oelt; \
	size_t i; \
	\
	for (i = 0; i < size; i++) { \
		ielt = in_array[i];			/*read the current element*/ \
		\
		if (ielt >= MIDDLE) 		/*if it has a positive value as fp number*/ \
			oelt.i = ielt - MIDDLE; \
		else \
			oelt.i = END - ielt; \
		\
		out_array[i] = oelt.fp; \
		} \
	\
	return 0; \
}

extern int8_t uint32_to_float
	UINT_TO_FP(float, uint32_t, 0x80000000, 0xFFFFFFFF)
extern int8_t uint64_to_double
	UINT_TO_FP(double, uint64_t, 0x8000000000000000, 0xFFFFFFFFFFFFFFFF)

#undef UINT_TO_FP
