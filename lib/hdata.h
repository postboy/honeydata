/*
hdata.h - honeydata library header
License: BSD 2-Clause
*/

#include <stdio.h>
#include <inttypes.h>

#include <openssl/bn.h>

//DTE and DTD for unsigned 8 bit integers distributed uniformly
extern char encode_uint8_uniform(const unsigned char *in_array, unsigned char *out_array,
	const uint8_t min, const uint8_t max, const uint64_t count);
extern char decode_uint8_uniform(const unsigned char *in_array, unsigned char *out_array,
	const uint8_t min, const uint8_t max, const uint64_t count);
