/*
hd_int_uniform.c - components for integers distributed uniformly
License: BSD 2-Clause
*/

#include "hd_int_uniform.h"

//DTE for unsigned 8 bit integers distributed uniformly; returns 0 on success or -1 on error
extern int8_t encode_uint8_uniform(const unsigned char *in_array, unsigned char *out_array,
	const uint8_t min, const uint8_t max, const uint64_t count)
{

	uint64_t i;			//cycle counter
	uint8_t elt;		//current processing element
	//maximum offset of normalized values, so they will have values in interval [0; max_offset]
	const uint8_t max_offset = max - min,
	//maximum number of a group, so they will have values in interval [0; max_group]
	max_group = UINT8_MAX / max_offset - 1,
	/*number of elements in the last group or 0 if the last group is full, i.e. it contains
	'max_offset' elements*/
	last_group_size = UINT8_MAX % max_offset;
	
	//wrong input values
	if (min > max) {
		fprintf(stderr, "hd_int_uniform: encode_uint8_uniform error: min > max\n");
		return -1;
		}
	if (count < 1){
		fprintf(stderr, "hd_int_uniform: encode_uint8_uniform error: count < 1\n");
		return -1;
		}
	
	//if every value is possible then just copy input array to output array
	if ((min == 0) && (max == UINT8_MAX)) {
		memcpy((void *)in_array, out_array, count);
		return 0;
		}


	//write a random numbers to output array
	if (!RAND_bytes(out_array, count)) {
    	ERR_print_errors_fp(stderr);
    	return -1;
    	}

	/*if only one value is possible then use a random number for encoding each number, so we're
	already done!*/
	if (min == max)
		return 0;
	
	//else encode each number using random numbers from out_array for group selection
	for (i = 0; i < count; i++) {
		elt = (uint8_t)(*in_array+i) - min;	//read current value and normalize it
		
		//if we can place a current element in any group including the last one then do it
		if ( (elt < last_group_size) || (last_group_size == 0) )
			elt += ( (uint8_t)(*out_array+i) % max_group ) * max_offset;
		//else place it in any group excluding the last one
		else
			elt += ( (uint8_t)(*out_array+i) % (max_group-1) ) * max_offset;
		memcpy(&elt, (out_array+i), 1);	//finally write it to buffer
		}

	return 0;
}

//DTD for unsigned 8 bit integers distributed uniformly; returns 0 on success or -1 on error
extern int8_t decode_uint8_uniform(const unsigned char *in_array, unsigned char *out_array,
	const uint8_t min, const uint8_t max, const uint64_t count)
{
	return 0;
}
