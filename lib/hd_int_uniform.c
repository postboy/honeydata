/*
hd_int_uniform.c - components for integers distributed uniformly
License: BSD 2-Clause
*/

#include "hd_int_uniform.h"

//reduce a secret (e.g. a key or a password) to one-byte variable for encoding randomization
extern int8_t reduce_secret_to_1byte(const unsigned char *secret, const uint32_t secret_size,
	uint8_t *reduction)
{
	EVP_MD_CTX *mdctx;
	const uint8_t SHA224_HSIZE = 28;	//size of SHA-224 hash in bytes	
	//hash of secret, temporary byte for calculationg a reduction function, cycle counter
	unsigned char hash[SHA224_HSIZE], tmp, i;
	unsigned int hash_len;				//it's length

	//wrong input value
	if (secret_size < 1) {
		fprintf(stderr, "hd_int_uniform: reduce_secret error: secret_size < 1\n");
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

	if (EVP_DigestUpdate(mdctx, secret, secret_size) != 1) {
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
		if ( (i+1) % 4 == 0 ) printf(" ");	//place a space every 2 bytes
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

//get minimum and maximum of uint8 array
extern int8_t get_uint8_array_metadata(const unsigned char *array, const uint64_t size,
	uint8_t *min, uint8_t *max)
{
	uint8_t tmpmin, tmpmax;	//variables for storing temporary minimum and maximum values
	uint64_t i;				//cycle counter

	//wrong input value
	if (size < 1) {
		fprintf(stderr, "hd_int_uniform: get_uint8_array_metadata error: size < 1\n");
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
		if ( (tmpmin == 0) && (tmpmax == 256) )	//we've already got minimum and maximum of array
			break;
		}
	
	//finally copy results to output buffers
	memcpy(min, &tmpmin, 1);
	memcpy(max, &tmpmax, 1);
	
	return 0;
}

//DTE for unsigned 8 bit integers distributed uniformly
extern int8_t encode_uint8_uniform(const unsigned char *in_array, unsigned char *out_array,
	const uint8_t min, const uint8_t max, const uint8_t reduction, const uint64_t size)
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

	uint64_t i;									//cycle counter
	uint8_t elt;								//current processing element
	const uint16_t group_size = max - min + 1;	//size of a full group in elements, from 1 to 256
	//number of groups, so they will have values in interval [0; group_num-1], from 0 to 255
	const uint8_t group_num = ceil(256 / (group_size+0.0)),
	//number of elements in the last group or 0 if the last group is full, from 0 to 127
	last_group_size = 256 % group_size;
	
	
	//if every value is possible
	if (group_size == 256) {
		//then just copy input array content to output array
		memcpy(out_array, in_array, size);
		return 0;
		}

	//write a random numbers to output array
	if (!RAND_bytes(out_array, size)) {
    	ERR_print_errors_fp(stderr);
    	return 1;
    	}

	//if only one value is possible then use a random number for encoding each number
	if (group_size == 1) {
		//we're already done with random numbers, but we should check an input array
		for (i = 0; i < size; i++) {
			elt = (uint8_t)in_array[i];	//read current value
			if (elt != min) {
				fprintf(stderr, "hd_int_uniform: encode_uint8_uniform error: wrong min or max value\n");
				return 1;
				}
			}
		return 0;
		}
	
	//else encode each number using random numbers from out_array for group selection
	for (i = 0; i < size; i++) {
		elt = (uint8_t)in_array[i];				//read current value
		if ( (elt < min) || (elt > max) ) {
			fprintf(stderr, "hd_int_uniform: encode_uint8_uniform error: wrong min or max value\n");
			return 1;
			}
		elt = elt - min;						//normalize it
		elt = (elt + reduction) % group_size;	//add a randomozing offset to it

		//if we can place a current element in any group (including the last one) then do it
		if ( (elt < last_group_size) || (last_group_size == 0) )
			elt += ( (uint8_t)out_array[i] % group_num ) * group_size;
		//else place it in any group excluding the last one
		else
			elt += ( (uint8_t)out_array[i] % (group_num-1) ) * group_size;
			
		out_array[i] = elt;	//finally write it to buffer
		}

	return 0;
}

//DTD for unsigned 8 bit integers distributed uniformly
extern int8_t decode_uint8_uniform(const unsigned char *in_array, unsigned char *out_array,
	const uint8_t min, const uint8_t max, const uint8_t reduction, const uint64_t size)
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

	uint64_t i;									//cycle counter
	uint8_t elt;								//current processing element
	const uint16_t group_size = max - min + 1;	//size of a full group in elements, from 1 to 256
	
	//if every value is possible
	if (group_size == 256) {
		//then just copy input array content to output array
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
		elt = (uint8_t)in_array[i];	//read current value
		elt = elt % group_size;		//get a value in group
		if (elt >= reduction)		//substitute a randomozing offset from it
			elt = elt - reduction;
		else if ( (reduction/group_size)*group_size + elt >= reduction)
			elt = (reduction/group_size)*group_size + elt - reduction;
		else
			elt = (reduction/group_size + 1)*group_size + elt - reduction;
		elt = elt + min; 			//denormalize it
		
		//if algorithm works right, this error should never been thrown
		if ( (elt < min) || (elt > max) ) {
			fprintf(stderr, "hd_int_uniform: decode_uint8_uniform error: algorithm error\n");
			return 1;
			}
		
		out_array[i] = elt;	//finally write it to buffer
		}

	return 0;
}
