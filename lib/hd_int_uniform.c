/*
hd_int_uniform.c - components for integers distributed uniformly
License: BSD 2-Clause
*/

#include "hd_int_uniform.h"

//reduce a secret (e.g. a key or a password) to one-byte variable for encoding randomization
extern int8_t reduce_secret_to_1byte(const unsigned char *secret, uint32_t secret_len,
	unsigned char *result)
{
	EVP_MD_CTX *mdctx;
	const uint8_t SHA224_HSIZE = 28;	//size of SHA-224 hash in bytes	
	//hash of secret, temporary byte for calculationg a reduction function, cycle counter
	unsigned char hash[SHA224_HSIZE], tmp, i;
	unsigned int hash_len;				//it's length

	//wrong input value
	if (secret_len < 1) {
		fprintf(stderr, "hd_int_uniform: reduce_secret error: secret_len < 1\n");
		return 1;
		}

	//hash the secret value
	if ((mdctx = EVP_MD_CTX_create()) == NULL) {
		ERR_print_errors_fp(stderr);
		return 1;
		}

	if (EVP_DigestInit_ex(mdctx, EVP_sha224(), NULL) != 1) {
		ERR_print_errors_fp(stderr);
		return 1;
		}

	if (EVP_DigestUpdate(mdctx, secret, secret_len) != 1) {
		ERR_print_errors_fp(stderr);
		return 1;
		}

	if (EVP_DigestFinal_ex(mdctx, hash, &hash_len) != 1) {
		ERR_print_errors_fp(stderr);
		return 1;
		}
		
	EVP_MD_CTX_destroy(mdctx);
	
	//check if hash has a proper size
	if (hash_len != SHA224_HSIZE) {
		fprintf(stderr, "hd_int_uniform: reduce_secret error: hash_len != SHA224_HSIZE\n");
		return 1;
		}

	//rediction function is a XOR of all bytes of SHA-224 hash
	tmp = hash[0];
	for (i = 1; i < SHA224_HSIZE; i++)
		tmp = tmp ^	hash[i];
	
	memcpy(result, &tmp, 1);	//finally copy reduction result to output buffer
	
	return 0;
}

//DTE for unsigned 8 bit integers distributed uniformly; returns 0 on success or 1 on error
extern int8_t encode_uint8_uniform(const unsigned char *in_array, unsigned char *out_array,
	const uint8_t min, const uint8_t max, const uint64_t count)
{

	uint64_t i;									//cycle counter
	uint8_t elt;								//current processing element
	//size of a full group in elements, from 1 to 256
	const uint16_t group_size = max - min + 1;
	//maximum number of a group, so they will have values in interval [0; max_group], from 0 to 255
	const uint8_t max_group = ceil(256 / (group_size+0.0)) - 1,
	//number of elements in the last group or 0 if the last group is full, from 0 to 127
	last_group_size = 256 % group_size;
	
	//wrong input values
	if (min > max) {
		fprintf(stderr, "hd_int_uniform: encode_uint8_uniform error: min > max\n");
		return 1;
		}
	if (count < 1){
		fprintf(stderr, "hd_int_uniform: encode_uint8_uniform error: count < 1\n");
		return 1;
		}
	
	//if every value is possible then just copy input array to output array
	if ((min == 0) && (max == UINT8_MAX)) {
		memcpy((void *)in_array, out_array, count);
		return 0;
		}

	//write a random numbers to output array
	if (!RAND_bytes(out_array, count)) {
    	ERR_print_errors_fp(stderr);
    	return 1;
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
			elt += ( (uint8_t)(*out_array+i) % max_group ) * group_size;
		//else place it in any group excluding the last one
		else
			elt += ( (uint8_t)(*out_array+i) % (max_group-1) ) * group_size;
		memcpy(&elt, (out_array+i), 1);	//finally write it to buffer
		}

	return 0;
}

//DTD for unsigned 8 bit integers distributed uniformly; returns 0 on success or 1 on error
extern int8_t decode_uint8_uniform(const unsigned char *in_array, unsigned char *out_array,
	const uint8_t min, const uint8_t max, const uint64_t count)
{
	return 0;
}
