/*
hd_int_uniform.c - components for integers distributed uniformly
License: BSD 2-Clause
*/

#include "hd_int_uniform.h"

//get minimum and maximum of uint8 array
extern int8_t get_uint8_array_minmax(const uint8_t *array, const uint64_t size,
	uint8_t *min, uint8_t *max)
{
	uint8_t tmpmin, tmpmax;	//variables for storing temporary minimum and maximum values
	uint64_t i;

	//wrong input value
	if (size < 1) {
		fprintf(stderr, "hd_int_uniform: get_uint8_array_minmax error: size < 1\n");
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
		/*we don't break a cycle if ((tmpmin == 0) && (tmpmax == 256)) because it can be used for
		timing attack*/
		}
	
	//finally copy results to output buffers
	memcpy(min, &tmpmin, 1);
	memcpy(max, &tmpmax, 1);
	
	return 0;
}

//DTE for unsigned 8 bit integers distributed uniformly
extern int8_t encode_uint8_uniform(const uint8_t *in_array, uint16_t *out_array,
	const uint8_t min, const uint8_t max, const uint64_t size)
{
	//wrong input values
	if (min > max) {
		fprintf(stderr, "hd_int_uniform: encode_uint8_uniform error: min > max\n");
		return 1;
		}
	if (size < 1) {
		fprintf(stderr, "hd_int_uniform: encode_uint8_uniform error: size < 1\n");
		return 1;
		}

	uint64_t i;
	uint16_t elt;								//current processing element
	const uint16_t group_size = max - min + 1;	//size of a full group in elements, from 1 to 256
	/*number of groups (from 256 to 65536), so they will have values in interval [0; group_num-1];
	notice type conversion here - we want float result of division instead of integer!*/
	const uint32_t group_num = ceil(65536 / (float)group_size);
	//number of elements in the last group or 0 if the last group is full, from 0 to 255
	const uint8_t last_group_size = 65536 % group_size;
	
	//if every value is possible
	if (group_size == 256) {
		//then just copy input array to output array to create a first half
		memcpy(out_array, in_array, size);
		//follow it by random numbers to create a second half
		if (!RAND_bytes((unsigned char *)(out_array+size), size)) {
    		ERR_print_errors_fp(stderr);
    		return 1;
    		}
		return 0;
		}

	//write a random numbers to output array
	if (!RAND_bytes((unsigned char *)out_array, 2*size)) {
    	ERR_print_errors_fp(stderr);
    	return 1;
    	}

	//if only one value is possible then use a random number for encoding each number
	if (group_size == 1) {
		//we're already done with random numbers, but we should check an input array
		for (i = 0; i < size; i++) {
			if (in_array[i] != min) {
				fprintf(stderr, "hd_int_uniform: encode_uint8_uniform error: wrong min or max value\n");
				return 1;
				}
			}
		return 0;
		}
	
	//else encode each number using random numbers from out_array for group selection
	for (i = 0; i < size; i++) {
		elt = in_array[i];	//read current value
		if ( (elt < min) || (elt > max) ) {
			fprintf(stderr, "hd_int_uniform: encode_uint8_uniform error: wrong min or max value\n");
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
extern int8_t decode_uint8_uniform(const uint16_t *in_array, uint8_t *out_array,
	const uint8_t min, const uint8_t max, const uint64_t size)
{
	//wrong input values
	if (min > max) {
		fprintf(stderr, "hd_int_uniform: decode_uint8_uniform error: min > max\n");
		return 1;
		}
	if (size < 1) {
		fprintf(stderr, "hd_int_uniform: decode_uint8_uniform error: size < 1\n");
		return 1;
		}

	uint64_t i;
	uint16_t elt;								//current processing element
	const uint16_t group_size = max - min + 1;	//size of a full group in elements, from 1 to 256
	
	//if every value is possible
	if (group_size == 256) {
		//then just copy first half of input array to output array
		memcpy(out_array, in_array, size);
		return 0;
		}

	//if only one value is possible then write this value 'size' times
	if (group_size == 1) {
		memset(out_array, min, size);
		return 0;
		}
	
	//else decode each number
	for (i = 0; i < size; i++) {
		//read current element, get it's value in first group, denormalize it
		elt = (in_array[i] % group_size) + min;
		
		//if algorithm works right, this error should never been thrown
		if ( (elt < min) || (elt > max) ) {
			fprintf(stderr, "hd_int_uniform: decode_uint8_uniform error: algorithm error\n");
			return 1;
			}
		
		out_array[i] = (uint8_t)elt;	//finally write it to buffer
		}

	return 0;
}
