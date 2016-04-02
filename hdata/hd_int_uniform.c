/*
hd_int_uniform.c - components for integers distributed uniformly
License: BSD 2-Clause
*/

#include "hd_int_uniform.h"

//uint8_t functions--------------------------------------------------------------------------------

#define itype uint8_t	//type of input elements
#define utype uint8_t	//unsigned type of same size as input type
//type of output elements (always unsigned, size is twice larger than size of itype)
#define otype uint16_t
#define ISPACE (UINT8_MAX+1)	//size of utype code space
#define OSPACE (UINT16_MAX+1)	//size of otype code space

//get minimum and maximum of uint8 array
extern int8_t get_uint8_array_minmax(const itype *array, const uint64_t size,
	itype *min, itype *max)
{
	itype tmpmin, tmpmax;	//variables for storing temporary minimum and maximum values
	uint64_t i;

	//wrong input value
	if (size < 1) {
		error("size < 1");
		return 1;
		}

	//initialize minimum and maximum values
	tmpmin = array[0];
	tmpmax = array[0];
	//let's try to find smaller minimum and bigger maximum
	for (i = 1; i < size; i++) {
		if (array[i] < tmpmin)					//then it's the new minimum
			tmpmin = array[i];
		if (array[i] > tmpmax)					//then it's the new maximum
			tmpmax = array[i];
		/*we don't break a cycle if ((tmpmin == itype_MIN) && (tmpmax == itype_MAX)) because it can
		be used for timing attack*/
		}
	
	//finally copy results to output buffers
	*min = tmpmin;
	*max = tmpmax;
	
	return 0;
}

//DTE for unsigned 8 bit integers distributed uniformly
extern int8_t encode_uint8_uniform(const itype *in_array, otype *out_array,
	const itype min, const itype max, const uint64_t size)
{
	//wrong input values
	if (size < 1) {
		error("size < 1");
		return 1;
		}
	if (min > max) {
		error("min > max");
		return 1;
		}

	uint64_t i;
	otype elt;								//current processing element
	const otype group_size = max - min + 1;
	//size of a full group in elements, from 1 to (itype_MAX-itype_MIN+1)
	/*number of groups (from 256 to 65536), so they will have values in interval [0; group_num-1];
	notice type conversion here - we want float result of division instead of integer!*/
	const uint32_t group_num = ceil(OSPACE / (float)group_size);
	//number of elements in the last group or 0 if the last group is full, from 0 to 255
	const utype last_group_size = OSPACE % group_size;
	
	//if every value is possible
	if (group_size == ISPACE) {
		//then just copy input array to output array to create a first half
		memcpy(out_array, in_array, size*sizeof(itype));
		//follow it by random numbers to create a second half
		if (!RAND_bytes( (unsigned char *)out_array + size*sizeof(itype), size*sizeof(itype) )) {
    		ERR_print_errors_fp(stderr);
    		return 1;
    		}
		return 0;
		}

	//write a random numbers to output array
	if (!RAND_bytes( (unsigned char *)out_array, 2*size*sizeof(itype) )) {
    	ERR_print_errors_fp(stderr);
    	return 1;
    	}

	//if only one value is possible then use a random number for encoding each number
	if (group_size == 1) {
		//we're already done with random numbers, but we should check an input array
		for (i = 0; i < size; i++) {
			if (in_array[i] != min) {
				error("wrong min or max value");
				return 1;
				}
			}
		return 0;
		}
	
	//else encode each number using random numbers from out_array for group selection
	for (i = 0; i < size; i++) {
		elt = in_array[i];	//read current value
		if ( (elt < min) || (elt > max) ) {
			error("wrong min or max value");
			return 1;
			}
		elt = elt - min;	//normalize it

		//if we can place a current element in any group (including the last one) then do it
		if ( (elt < last_group_size) || (last_group_size == 0) )
			elt += (out_array[i] % group_num) * group_size;
		//else place it in any group excluding the last one
		else
			elt += ( out_array[i] % (group_num-1) ) * group_size;
			
		out_array[i] = elt;	//finally write it to buffer
		}

	return 0;
}

//DTD for unsigned 8 bit integers distributed uniformly
extern int8_t decode_uint8_uniform(const otype *in_array, itype *out_array,
	const itype min, const itype max, const uint64_t size)
{
	//wrong input values
	if (size < 1) {
		error("size < 1");
		return 1;
		}
	if (min > max) {
		error("min > max");
		return 1;
		}

	uint64_t i;
	otype elt;								//current processing element
	//size of a full group in elements, from 1 to (itype_MAX-itype_MIN+1)
	const otype group_size = max - min + 1;
	
	//if every value is possible then just copy first half of input array to output array
	if (group_size == ISPACE) {
		memcpy(out_array, in_array, size*sizeof(itype));
		return 0;
		}

	//if only one value is possible then fill output array with this value
	if (group_size == 1) {
		for (i = 0; i < size; i++)
			out_array[i] = min;
		return 0;
		}
	
	//else decode each number
	for (i = 0; i < size; i++) {
		//read current element, get it's value in first group, denormalize it
		elt = (in_array[i] % group_size) + min;
		
		//if algorithm works right, this error should never been thrown
		if ( (elt < min) || (elt > max) ) {
			error("algorithm error");
			return 1;
			}
		
		out_array[i] = (itype)elt;	//finally write it to buffer
		}

	return 0;
}

#undef itype
#undef utype
#undef otype
#undef ISPACE
#undef OSPACE