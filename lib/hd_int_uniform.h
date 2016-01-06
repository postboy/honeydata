/*
hd_int_uniform.h - components for integers distributed uniformly
License: BSD 2-Clause
*/

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

#include "poison.h"

//reduce a secret (e.g. a key or a password) to one-byte variable for encoding randomization
extern int8_t reduce_secret_to_1byte(const unsigned char *secret, const uint32_t secret_len,
	uint8_t *reduction);

//DTE and DTD for unsigned 8 bit integers distributed uniformly; return 0 on success or 1 on error
extern int8_t encode_uint8_uniform(const unsigned char *in_array, unsigned char *out_array,
	const uint8_t min, const uint8_t max, const uint8_t reduction, const uint64_t size);
extern int8_t decode_uint8_uniform(const unsigned char *in_array, unsigned char *out_array,
	const uint8_t min, const uint8_t max, const uint8_t reduction, const uint64_t size);
