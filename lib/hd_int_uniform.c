/*
hd_int_uniform.c - components for integers distributed uniformly
License: BSD 2-Clause
*/

#include "hd_int_uniform.h"

//reduce a secret (e.g. a key or a password) to one-byte variable for encoding randomization
extern int8_t reduce_secret_to_1byte(const unsigned char *secret, const uint32_t secret_len,
	uint8_t *reduction)
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
	
	/*
	//for debugging purposes: output of SHA-224 hash
	//SHA224("The quick brown fox jumps over the lazy dog") =
	//0x730e109b d7a8a32b 1cb9d9a0 9aa2325d 2430587d dbc0c38b ad911525
	
	char temp[4];	//buffer for temporary output

	for (i=0; i < SHA224_HSIZE; i++) {
		snprintf(temp, 4, "%02x", hash[i]);	//output format of hash is HEX-code
		printf("%s", temp);
	
		//place a space every 2 bytes
		if ((i+1) % 4 == 0) {
			snprintf(temp, 2, " ");
			printf("%s", temp);
			}
		
    	}	//for

    printf("\n");
    */

	//rediction function is a XOR of all bytes of SHA-224 hash
	tmp = hash[0];
	for (i = 1; i < SHA224_HSIZE; i++)
		tmp = tmp ^	hash[i];
	
	memcpy(reduction, &tmp, 1);	//finally copy reduction result to output buffer
	
	return 0;
}

//DTE for unsigned 8 bit integers distributed uniformly; returns 0 on success or 1 on error
extern int8_t encode_uint8_uniform(const unsigned char *in_array, unsigned char *out_array,
	const uint8_t min, const uint8_t max, const uint8_t reduction, const uint64_t count)
{
	//wrong input values
	if (min > max) {
		fprintf(stderr, "hd_int_uniform: encode_uint8_uniform error: min > max\n");
		return 1;
		}
	if (count < 1){
		fprintf(stderr, "hd_int_uniform: encode_uint8_uniform error: count < 1\n");
		return 1;
		}

	uint64_t i;									//cycle counter
	uint8_t elt;								//current processing element
	const uint16_t group_size = max - min + 1;	//size of a full group in elements, from 1 to 256
	//maximum number of a group, so they will have values in interval [0; max_group], from 0 to 255
	const uint8_t max_group = ceil(256 / (group_size+0.0)) - 1,
	//number of elements in the last group or 0 if the last group is full, from 0 to 127
	last_group_size = 256 % group_size;
	
	//if every value is possible then just copy input array to output array
	if (group_size == 256) {
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
	if (group_size == 1)
		return 0;
	
	//else encode each number using random numbers from out_array for group selection
	for (i = 0; i < count; i++) {
		//read current value, normalize it, add a randomozing offset to every number
		elt = ( (uint8_t)(*in_array+i) - min + reduction) % group_size;
		
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
	const uint8_t min, const uint8_t max, const uint8_t reduction, const uint64_t count)
{
	//wrong input values
	if (min > max) {
		fprintf(stderr, "hd_int_uniform: decode_uint8_uniform error: min > max\n");
		return 1;
		}
	if (count < 1){
		fprintf(stderr, "hd_int_uniform: decode_uint8_uniform error: count < 1\n");
		return 1;
		}

	uint64_t i;									//cycle counter
	uint8_t elt;								//current processing element
	const uint16_t group_size = max - min + 1;	//size of a full group in elements, from 1 to 256
	
	//if every value is possible then just copy input array to output array
	if (group_size == 256) {
		memcpy((void *)in_array, out_array, count);
		return 0;
		}

	//if only one value is possible then write this value 'count' times
	if (group_size == 1) {
		memset(out_array, min, count);
		return 0;
		}
	
	//else decode each number
	for (i = 0; i < count; i++) {
		//read current value, normalize it, add a randomozing offset to every number
		elt = ( (uint8_t)(*in_array+i) + min) % group_size;
		if (elt <= reduction)
			elt = elt - reduction;
		else
			elt = UINT8_MAX + elt - reduction;
		
		memcpy(&elt, (out_array+i), 1);	//finally write it to buffer
		}

	return 0;
}
