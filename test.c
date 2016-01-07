/*
test.c - test program for honeydata library
License: BSD 2-Clause
*/

#include <openssl/conf.h>

#include "lib/hd_int_uniform.h"

static int8_t print_uint8_array(const uint8_t *array, const uint64_t size)
{
	uint64_t i;	//cycle counter
	
	//wrong input value
	if (size < 1) {
		fprintf(stderr, "test: print_uint8_array error: size < 1\n");
		return 1;
		}
	
	for (i = 0; i < size; i++)
		printf("%i ", array[i]);
	printf("\n");
	return 0;
}

static void error_handler(void)
{
	ERR_print_errors_fp(stderr);
	exit(1);
}

//encrypt a message
static int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv,
	unsigned char *ciphertext)
{
	EVP_CIPHER_CTX *ctx;

	int len, ciphertext_len;

	//create and initialise the context
	if (!(ctx = EVP_CIPHER_CTX_new()))
		error_handler();

	/*Initialise the encryption operation. IMPORTANT - ensure you use a key and IV size
	appropriate for your cipher. In this example we are using 256 bit AES (i.e. a 256 bit key). The
	IV size for *most* modes is the same as the block size. For AES this is 128 bits.*/
	if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1)
		error_handler();

	/*Provide the message to be encrypted, and obtain the encrypted output. EVP_EncryptUpdate can
	be called multiple times if necessary.*/
	if (EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len) != 1)
		error_handler();
	ciphertext_len = len;

	//Finalise the encryption. Further ciphertext bytes may be written at this stage.
	if (EVP_EncryptFinal_ex(ctx, ciphertext + len, &len) != 1)
		error_handler();
  	ciphertext_len += len;

	//clean up
	EVP_CIPHER_CTX_free(ctx);

	return ciphertext_len;
}

//decrypt a message
static int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv,
	unsigned char *plaintext)
{
	EVP_CIPHER_CTX *ctx;

	int len, plaintext_len;

	//create and initialise the context
	if (!(ctx = EVP_CIPHER_CTX_new()))
		error_handler();

	/*Initialise the decryption operation. IMPORTANT - ensure you use a key and IV size appropriate
	for your cipher. In this example we are using 256 bit AES (i.e. a 256 bit key). The IV size for
	*most* modes is the same as the block size. For AES this is 128 bits.*/
	if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1)
		error_handler();

	/*Provide the message to be decrypted, and obtain the plaintext output. EVP_DecryptUpdate can
	be called multiple times if necessary.*/
	if (EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len) != 1)
		error_handler();
	plaintext_len = len;

	//Finalise the decryption. Further plaintext bytes may be written at this stage.
	if (EVP_DecryptFinal_ex(ctx, plaintext + len, &len) != 1)
		error_handler();
	plaintext_len += len;

	//clean up
	EVP_CIPHER_CTX_free(ctx);

	return plaintext_len;
}

//get a number of occurences of different bytes in array
static int8_t array_statistics(const unsigned char *in_array, const uint64_t size, uint64_t *stats)
{
	uint64_t i;			//cycle counter
	unsigned char elt;	//current processing element
	
	//wrong input value
	if (size < 1) {
		fprintf(stderr, "test: array_statistics error: size < 1\n");
		return 1;
		}
	
	memset(stats, 0, 8*256);	//initialize the output array
	for (i = 0; i < size; i++) {
		elt = in_array[i];		//read a current element
		++stats[elt];			//increment the corresponding number in output array
		}
		
	return 0;
}

extern int main(void)
{
	//Set up the key and IV. Do I need to say to not hard code these in a real application? :-)
	unsigned char *key = (unsigned char *)"01234567890123456789012345678901";	//a 256 bit key
	unsigned char *iv = (unsigned char *)"01234567890123456";					//a 128 bit IV

	//initialise the crypto library
	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();
	OPENSSL_config(NULL);
	if (!RAND_status()) {
		fprintf(stderr, "test: RAND_status error: PRNG hasn't been seeded with enough data\n");
    	return -1;
		}

	//encoding tests-------------------------------------------------------------------------------
	
	uint8_t reduction, min, max,	//reduction result, minimum and maximim
	orig_array[65536] = {20, 20, 20, 20, 20}, encoded_array[65536], decoded_array[65536];
	//cycle counter, array size, statistics on pseudorandom and output arrays
	uint64_t i, size = 65536, in_stats[256], out_stats[256];
	
	reduce_secret_to_1byte(key, 32, &reduction);
	
	//random encoding and decoding with statistics
	if (!RAND_bytes(orig_array, size)) {	//write a random numbers to original array
    	ERR_print_errors_fp(stderr);
    	return 1;
    	}
    array_statistics(orig_array, size, in_stats);	//get a statistics on a pseudorandom numbers
    
    //let orig_array contain numbers from 120 to 239
	for (i = 0; i < size; i++)
		orig_array[i] = (orig_array[i] % 120) + 120;
	encode_uint8_uniform(orig_array, encoded_array, 120, 239, i, size);
	array_statistics(encoded_array, size, out_stats);	//get a statistics on an encoded array
    decode_uint8_uniform(encoded_array, decoded_array, 120, 239, i, size);
    if (memcmp(orig_array, decoded_array, size)) {
		print_uint8_array(orig_array, size);
		print_uint8_array(decoded_array, size);
		}
	//three columns: ideal, pseudorandom and actual distribution for chi2test
	for (i = 0; i < 256; i++)
		printf("%llu\t%llu\t%llu\n", in_stats[i], out_stats[i], size/256);
	
	/*
	//wrong parameters
	reduce_secret_to_1byte(NULL, 0, NULL);
	get_uint8_array_metadata(NULL, 0, NULL, NULL);
	encode_uint8_uniform(NULL, NULL, 2, 1, 0, 1);
	encode_uint8_uniform(NULL, NULL, 1, 2, 0, 0);
	encode_uint8_uniform(orig_array, encoded_array, 100, 100, 0, size);
	encode_uint8_uniform(orig_array, encoded_array, 21, 100, 0, size);
	decode_uint8_uniform(NULL, NULL, 2, 1, 0, 1);
	decode_uint8_uniform(NULL, NULL, 1, 2, 0, 0);
	print_uint8_array(NULL, 0);
	array_statistics(NULL, 0, NULL);
	printf("\n");
	*/
	
	/*
	size = 5;
	
	//fixed special cases
	
	printf("Original array:\n");
	print_uint8_array(orig_array, size);
	get_uint8_array_metadata(orig_array, size, &min, &max);
	
	printf("min = %i, max = %i, reduction = 0:\n", min, max);
	encode_uint8_uniform(orig_array, encoded_array, min, max, 0, size);
	print_uint8_array(encoded_array, size);
	decode_uint8_uniform(encoded_array, decoded_array, min, max, 0, size);
	//if original and decoded arrays is not equal then print a decoded array too
	if (memcmp(orig_array, decoded_array, size))
		print_uint8_array(decoded_array, size);
	
	printf("min = %i, max = %i, reduction = %i:\n", min, max, reduction);
	encode_uint8_uniform(orig_array, encoded_array, min, max, reduction, size);
	print_uint8_array(encoded_array, size);
	decode_uint8_uniform(encoded_array, decoded_array, min, max, reduction, size);
	if (memcmp(orig_array, decoded_array, size))
		print_uint8_array(decoded_array, size);
	
	printf("min = 0, max = 255, reduction = 0:\n");
	encode_uint8_uniform(orig_array, encoded_array, 0, 255, 0, size);
	print_uint8_array(encoded_array, size);
	decode_uint8_uniform(encoded_array, decoded_array, 0, 255, 0, size);
	if (memcmp(orig_array, decoded_array, size))
		print_uint8_array(decoded_array, size);
	
	printf("min = 0, max = 255, reduction = %i:\n", reduction);
	encode_uint8_uniform(orig_array, encoded_array, 0, 255, reduction, size);
	print_uint8_array(encoded_array, size);
	decode_uint8_uniform(encoded_array, decoded_array, 0, 255, reduction, size);
	if (memcmp(orig_array, decoded_array, size))
		print_uint8_array(decoded_array, size);
	printf("\n");
	*/
	
	/*
	//fixed general cases
	orig_array[0] = 20;
	orig_array[1] = 25;
	orig_array[2] = 30;
	orig_array[3] = 35;
	orig_array[4] = 40;
	
	printf("Original array:\n");	//print it
	print_uint8_array(orig_array, size);
	get_uint8_array_metadata(orig_array, size, &min, &max);
	
	printf("min = %i, max = %i, reduction = 0:\n", min, max);
	encode_uint8_uniform(orig_array, encoded_array, min, max, 0, size);
	print_uint8_array(encoded_array, size);
	decode_uint8_uniform(encoded_array, decoded_array, min, max, 0, size);
	if (memcmp(orig_array, decoded_array, size))
		print_uint8_array(decoded_array, size);

	
	printf("min = %i, max = %i, reduction = %i:\n", min, max, reduction);
	encode_uint8_uniform(orig_array, encoded_array, min, max, reduction, size);
	print_uint8_array(encoded_array, size);
	decode_uint8_uniform(encoded_array, decoded_array, min, max, reduction, size);
	if (memcmp(orig_array, decoded_array, size))
		print_uint8_array(decoded_array, size);
	
	printf("min = 15, max = 45, reduction = 0:\n");
	encode_uint8_uniform(orig_array, encoded_array, 15, 45, 0, size);
	print_uint8_array(encoded_array, size);
	decode_uint8_uniform(encoded_array, decoded_array, 15, 45, 0, size);
	if (memcmp(orig_array, decoded_array, size))
		print_uint8_array(decoded_array, size);
	
	//reduction < 6 causes right decoding
	printf("min = 15, max = 45, reduction = %i:\n", reduction);
	encode_uint8_uniform(orig_array, encoded_array, 15, 45, reduction, size);
	print_uint8_array(encoded_array, size);
	decode_uint8_uniform(encoded_array, decoded_array, 15, 45, reduction, size);
	if (memcmp(orig_array, decoded_array, size))
		print_uint8_array(decoded_array, size);
	
	printf("min = 0, max = 255, reduction = 0:\n");
	encode_uint8_uniform(orig_array, encoded_array, 0, 255, 0, size);
	print_uint8_array(encoded_array, size);
	decode_uint8_uniform(encoded_array, decoded_array, 0, 255, 0, size);
	if (memcmp(orig_array, decoded_array, size))
		print_uint8_array(decoded_array, size);
	
	printf("min = 0, max = 255, reduction = %i:\n", reduction);
	encode_uint8_uniform(orig_array, encoded_array, 0, 255, reduction, size);
	print_uint8_array(encoded_array, size);
	decode_uint8_uniform(encoded_array, decoded_array, 0, 255, reduction, size);
	if (memcmp(orig_array, decoded_array, size))
		print_uint8_array(decoded_array, size);
	*/
	
	/*
	//random encoding and decoding
	for (i = 1; i < 256; i++) {
		if (!RAND_bytes(orig_array, i)) {	//write a random numbers to original array
    		ERR_print_errors_fp(stderr);
    		return 1;
    		}
    	get_uint8_array_metadata(orig_array, i, &min, &max);
		encode_uint8_uniform(orig_array, encoded_array, min, max, i, i);
    	decode_uint8_uniform(encoded_array, decoded_array, min, max, i, i);
    	if (memcmp(orig_array, decoded_array, i)) {
			print_uint8_array(orig_array, i);
			print_uint8_array(decoded_array, i);
			}
		}
	*/
	
	//encryption tests-----------------------------------------------------------------------------
	
	//message to be encrypted
	unsigned char *plaintext = (unsigned char *)"The quick brown fox jumps over the lazy dog";
	
	/*Buffer for ciphertext. Ensure the buffer is long enough for the ciphertext which may be
	longer than the plaintext, dependant on the algorithm and mode*/
	const uint8_t AES_BSIZE = 16;				//AES block size in bytes
	unsigned char ciphertext[3*AES_BSIZE];
	unsigned char decryptedtext[2*AES_BSIZE];	//buffer for the decrypted text
	uint32_t decryptedtext_len, ciphertext_len;	//their lengths
	
	//encrypt the plaintext
	ciphertext_len = encrypt(plaintext, strlen((char *)plaintext), key, iv, ciphertext);
	
	//decrypt the ciphertext
	decryptedtext_len = decrypt(ciphertext, ciphertext_len, key, iv, decryptedtext);
	
	//add a NULL terminator, because we are expecting printable text
	decryptedtext[decryptedtext_len] = '\0';

	//show the decrypted text
	//printf("%s\n", decryptedtext);

	//clean up
	RAND_cleanup();
	EVP_cleanup();
	ERR_free_strings();
	
	//getchar();	//for debugging purposes
	
	return 0;
}
