/*
hdata.c - honeydata library
License: BSD 2-Clause
*/

#include "hdata.h"

//DTE for unsigned 8 bit integers distributed uniformly
//returns a number of actually written bytes in out_array or -1 on error
extern uint64_t encode_uint8_uniform(const unsigned char *in_array, unsigned char *out_array,
	const uint8_t min, const uint8_t max, const int64_t count)
{

	uint64_t i;			//cycle counter
	uint16_t elt;		//current processing element
	unsigned char rnd;	//random bytes for group selection
	//maximum offset of normalized values, so they will have values in interval [0; max_offset]
	const uint8_t max_offset = max - min;
	//maximum number of a group, so they will have values in interval [0; max_group]
	const uint8_t max_group = UINT8_MAX / max_offset - 1;
	/*number of elements in the last group or 0 if the last group is full, i.e. it contains
	'max_offset' elements*/
	const uint8_t last_group_size = UINT8_MAX % max_offset;
	
	//wrong input values
	if (min > max) {
		fprintf(stderr, "encode_uint8_uniform error: min > max\n");
		return -1;
		}
	if (count < 1){
		fprintf(stderr, "encode_uint8_uniform error: count < 1\n");
		return -1;
		}
	
	//if every value is possible then just copy input array to output array
	if ((min == 0) && (max == UINT8_MAX)) {
		memcpy((void *)in_array, out_array, count);
		return count;	//we've written 'count' 1-byte elements
		}

	//if min == max then we use a random number of the same size for encoding each number
	if (min == max) {
		if (!RAND_bytes(out_array, count)) {
    		fprintf(stderr, "The PRNG is not seeded!\n");
    		return -1;
    		}
		return count;
		}
	
	//encoding itself
	for (i = 0; i < count; i++) {
		elt = (uint8_t)(*in_array+i) - min;	//read current value and normalize it
		
		//get a random number for group selection
		//TO DO: optimize it through reading multiple bytes at once!
		if (!RAND_bytes(&rnd, 1)) {
    		fprintf(stderr, "The PRNG is not seeded!\n");
    		return -1;
    		}
		
		//if we can place a current element in any group including the last one then do it
		if ((elt < last_group_size) || (last_group_size == 0))
			elt += (rnd % max_group) * max_offset;
		//else place it in any group excluding the last one
		else
			elt += (rnd % (max_group-1) ) * max_offset;
		memcpy(&elt, (out_array+2*i), 2);	//finally write it to buffer
		}

	return 2*count;	//we've written 'count' 2-byte elements
}

//DTD for unsigned 8 bit integers distributed uniformly
//returns a number of actually written bytes in out_array or -1 on error
extern uint64_t decode_uint8_uniform(const unsigned char *in_array, unsigned char *out_array,
	const uint8_t min, const uint8_t max, const int64_t count)
{
	return 0;
}
