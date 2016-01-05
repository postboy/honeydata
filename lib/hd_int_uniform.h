/*
hd_int_uniform.h - components for integers distributed uniformly
License: BSD 2-Clause
*/

#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include <openssl/rand.h>
#include <openssl/err.h>

#include "poison.h"

//DTE and DTD for unsigned 8 bit integers distributed uniformly; return 0 on success or -1 on error
extern int8_t encode_uint8_uniform(const unsigned char *in_array, unsigned char *out_array,
	const uint8_t min, const uint8_t max, const uint64_t count);
extern int8_t decode_uint8_uniform(const unsigned char *in_array, unsigned char *out_array,
	const uint8_t min, const uint8_t max, const uint64_t count);
