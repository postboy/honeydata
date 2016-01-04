/*
hdata.h - honeydata library header
License: BSD 2-Clause
*/

#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include <openssl/rand.h>

#include "poison.h"

//DTE and DTD for unsigned 8 bit integers distributed uniformly
//return a number of actually written bytes in out_array or -1 on error
extern uint64_t encode_uint8_uniform(const unsigned char *in_array, unsigned char *out_array,
	const uint8_t min, const uint8_t max, const int64_t count);
extern uint64_t decode_uint8_uniform(const unsigned char *in_array, unsigned char *out_array,
	const uint8_t min, const uint8_t max, const int64_t count);
