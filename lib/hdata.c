/*
hdata.c - honeydata library
License: BSD 2-Clause
*/

#include "hdata.h"

//DTE for unsigned 8 bit integers distributed uniformly
extern char encode_uint8_uniform(const unsigned char *in_array, unsigned char *out_array,
	const uint8_t min, const uint8_t max, const uint64_t count)
{
	uint64_t i;			//cycle counter
	uint8_t in_elt;		//current input element
	uint16_t out_elt;	//current output element
	
	if (min > max) {
		fprintf(stderr, "encode_uint8_uniform error: min > max\n");
		return 1;
		}		
	//maximum offset of normalized values, so they will have values in interval [0; max_offset]
	const uint8_t max_offset = max - min;
	
	for (i = 0; i < count; i++) {
		in_elt = (uint8_t)(*in_array+i) - min;	//read current value and normalize it
		//randomize it
		}
	
	return 0;
}

//DTD for unsigned 8 bit integers distributed uniformly
extern char decode_uint8_uniform(const unsigned char *in_array, unsigned char *out_array,
	const uint8_t min, const uint8_t max, const uint64_t count)
{
	return 0;
}
