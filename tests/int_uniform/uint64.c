/*
test program for honeydata library
license: BSD 2-Clause
*/

#include "../t_common.h"
#include "../../hdata/hd_int_uniform.h"

extern int main(void)
{
	
	//Set up the key and IV. Do I need to say to not hard code these in a real application? :-)
	unsigned char *key = (unsigned char *)"01234567890123456789012345678901";	//a 256 bit key
	unsigned char *iv = (unsigned char *)"01234567890123456";					//a 128 bit IV
	
	int32_t i;
	size_t size;									//current array size
	#define ITYPE uint64_t							//type for testing in this test unit
	#define PRI PRIu64								//macro for printing it
	#define OTYPE mpz_t								//type of container in this test unit
	#define BYTESIZE (size*sizeof(ITYPE))			//current input array size in bytes
	const size_t maxsize = 1048576/sizeof(ITYPE);	//maximum array size (1MB)
	ITYPE min, max, orig_array[maxsize], decoded_array[maxsize];	//minimum and maximim in array
	OTYPE encoded_array[maxsize];
	
	/*Buffers for plaintext and ciphertext. Ensure the buffer is long enough for the ciphertext
	which may be longer than the plaintext, dependant on the algorithm and mode (for AES-256 in CBC
	mode we need one extra block)*/
	unsigned char plaintext[2*256*sizeof(ITYPE)] = {0}, ciphertext[2*256*sizeof(ITYPE)];
	size_t decryptedtext_len, ciphertext_len;		//their lengths
	
	test_init();
	//initialize mpz_t numbers
	for (i = 0; i < maxsize; i++)
		mpz_init(encoded_array[i]);
	
	
	
	//random data encoding, encryption, decryption, decoding---------------------------------------
	
	size = 256;
	//write a random numbers to original array
	randombytes((unsigned char *)orig_array, BYTESIZE);
	
	//let orig_array contain numbers from 5000000000000000000 to 9999999999999999999
	for (i = 0; i < size; i++) {
		/*write a fresh random element to this position until it will be between 0 and
		14999999999999999999*/
		while (orig_array[i] > 14999999999999999999U)
			randombytes((unsigned char *)(orig_array+i), sizeof(ITYPE));
		orig_array[i] = (orig_array[i] % 5000000000000000000) + 5000000000000000000;
		}
	
	get_uint64_minmax(orig_array, size, &min, &max);
	encode_uint64_uniform(orig_array, encoded_array, size, min, max);
	//export array to plaintext array to get rid of GNU MP's service data
	for (i = 0; i < size; i++)
		mpz_export(plaintext+i*16, NULL, -1, sizeof(int), 0, 0, encoded_array[i]);
	ciphertext_len = encrypt(plaintext, 2*BYTESIZE, key, iv, ciphertext);
	decryptedtext_len = decrypt(ciphertext, ciphertext_len, key, iv, plaintext);
	//import array from plaintext array to use our data again
	for (i = 0; i < size; i++)
		mpz_import(encoded_array[i], 16/sizeof(int), -1, sizeof(int), 0, 0, plaintext+i*16);
	decode_uint64_uniform(encoded_array, decoded_array, size, min, max);
	
	//compare result of decryption and original array
	if ( memcmp(orig_array, decoded_array, BYTESIZE) || (decryptedtext_len != 2*BYTESIZE) ) {
		error("orig_array and decoded_array are not the same");
		printf("size = %zu, decryptedtext_len = %zu\n", size, decryptedtext_len);
		print_uint64_array(orig_array, 10);
		print_uint64_array(decoded_array, 10);
		test_error();
		}
	
	
	
	//random encoding and decoding-----------------------------------------------------------------
	
	for (size = 1; size < 256; size++) {
		//write a random numbers to original array
		randombytes((unsigned char *)orig_array, BYTESIZE);
		get_uint64_minmax(orig_array, size, &min, &max);
		encode_uint64_uniform(orig_array, encoded_array, size, min, max);
		decode_uint64_uniform(encoded_array, decoded_array, size, min, max);
		if (memcmp(orig_array, decoded_array, BYTESIZE)) {
			error("orig_array and decoded_array are not the same");
			print_uint64_array(orig_array, 10);
			print_uint64_array(decoded_array, 10);
			test_error();
			}
		}
	
	
	
	//fixed general cases--------------------------------------------------------------------------
	
	size = 5;
	orig_array[0] = 31000000000000;
	orig_array[1] = 32000000000000;
	orig_array[2] = 33000000000000;
	orig_array[3] = 34000000000000;
	orig_array[4] = 35000000000000;
	
	printf("Original array:\n");	//print it
	print_uint64_array(orig_array, size);
	get_uint64_minmax(orig_array, size, &min, &max);
	
	printf("min = %"PRI", max = %"PRI":\n", min, max);
	encode_uint64_uniform(orig_array, encoded_array, size, min, max);
	print_mpz_array(encoded_array, size);
	decode_uint64_uniform(encoded_array, decoded_array, size, min, max);
	//if original and decoded arrays are not equal then print a decoded array too
	if (memcmp(orig_array, decoded_array, BYTESIZE))
		print_uint64_array(decoded_array, size);
	
	printf("min = 10000000000000, max = 40000000000000:\n");
	encode_uint64_uniform(orig_array, encoded_array, size, 10000000000000, 40000000000000);
	print_mpz_array(encoded_array, size);
	decode_uint64_uniform(encoded_array, decoded_array, size, 10000000000000, 40000000000000);
	if (memcmp(orig_array, decoded_array, BYTESIZE))
		print_uint64_array(decoded_array, size);
	
	printf("min = 0, max = %"PRI":\n", UINT64_MAX);
	encode_uint64_uniform(orig_array, encoded_array, size, 0, UINT64_MAX);
	print_mpz_array(encoded_array, size);
	decode_uint64_uniform(encoded_array, decoded_array, size, 0, UINT64_MAX);
	if (memcmp(orig_array, decoded_array, BYTESIZE))
		print_uint64_array(decoded_array, size);
	printf("\n");
	
	
	//fixed special cases--------------------------------------------------------------------------
	
	size = 5;
	orig_array[0] = 16000000000000;
	orig_array[1] = 16000000000000;
	orig_array[2] = 16000000000000;
	orig_array[3] = 16000000000000;
	orig_array[4] = 16000000000000;
	
	printf("Original array:\n");
	print_uint64_array(orig_array, size);
	get_uint64_minmax(orig_array, size, &min, &max);
	
	printf("min = %"PRI", max = %"PRI":\n", min, max);
	encode_uint64_uniform(orig_array, encoded_array, size, min, max);
	print_mpz_array(encoded_array, size);
	decode_uint64_uniform(encoded_array, decoded_array, size, min, max);
	if (memcmp(orig_array, decoded_array, BYTESIZE))
		print_uint64_array(decoded_array, size);
	
	printf("min = 0, max = %"PRI":\n", UINT64_MAX);
	encode_uint64_uniform(orig_array, encoded_array, size, 0, UINT64_MAX);
	print_mpz_array(encoded_array, size);
	decode_uint64_uniform(encoded_array, decoded_array, size, 0, UINT64_MAX);
	if (memcmp(orig_array, decoded_array, BYTESIZE))
		print_uint64_array(decoded_array, size);
	printf("\n");
	
	
	
	//wrong parameters-----------------------------------------------------------------------------
	
	get_uint64_minmax(NULL, 0, NULL, NULL);
	get_uint64_minmax(orig_array, 0, NULL, NULL);
	get_uint64_minmax(orig_array, 1, NULL, NULL);
	get_uint64_minmax(orig_array, 1, &min, NULL);
	printf("\n");
	
	encode_uint64_uniform(NULL, NULL, 0, 0, 0);
	encode_uint64_uniform(orig_array, NULL, 0, 0, 0);
	encode_uint64_uniform(orig_array, encoded_array, 0, 0, 0);
	encode_uint64_uniform(orig_array, encoded_array, 1, 2, 1);
	encode_uint64_uniform(orig_array, encoded_array, size, 2600000000, 2700000000);
	encode_uint64_uniform(orig_array, encoded_array, size, 0, 1500000000);
	printf("\n");
	
	decode_uint64_uniform(NULL, NULL, 0, 0, 0);
	decode_uint64_uniform(encoded_array, NULL, 0, 0, 0);
	decode_uint64_uniform(encoded_array, orig_array, 0, 0, 0);
	decode_uint64_uniform(encoded_array, orig_array, 1, 2, 1);
	printf("\n");
	
	print_uint64_array(NULL, 0);
	print_uint64_array(orig_array, 0);
	printf("\n");
	
	print_mpz_array(NULL, 0);
	print_mpz_array(encoded_array, 0);
	
	
	
	#undef ITYPE
	#undef PRI
	#undef OTYPE
	#undef BYTESIZE
	test_deinit();
	//deinitialize mpz_t numbers
	for (i = 0; i < maxsize; i++)
		mpz_init(encoded_array[i]);
	
	return 0;
}
