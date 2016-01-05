/*
test.c - test program for honeydata library
License: BSD 2-Clause
*/

#include <openssl/conf.h>

#include "lib/hd_int_uniform.h"

static void error_handler(void)
{
	ERR_print_errors_fp(stderr);
	exit(1);
}

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

extern int main(void)
{
	//Set up the key and IV. Do I need to say to not hard code these in a real application? :-)
	unsigned char *key = (unsigned char *)"01234567890123456789012345678901";	//a 256 bit key
	unsigned char *iv = (unsigned char *)"01234567890123456";					//a 128 bit IV

	//message to be encrypted
	unsigned char *plaintext = (unsigned char *)"The quick brown fox jumps over the lazy dog";

	/*Buffer for ciphertext. Ensure the buffer is long enough for the ciphertext which may be
	longer than the plaintext, dependant on the algorithm and mode*/
	const uint8_t AES_BSIZE = 16;				//AES block size in bytes
	unsigned char ciphertext[4*AES_BSIZE];
	unsigned char decryptedtext[AES_BSIZE];		//buffer for the decrypted text
	uint32_t decryptedtext_len, ciphertext_len;	//their lengths

	//initialise the library
	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();
	OPENSSL_config(NULL);
	if (!RAND_status()) {
		fprintf(stderr, "test: RAND_status error: PRNG hasn't been seeded with enough data\n");
    	return -1;
		}

	//encoding tests-------------------------------------------------------------------------------
	
	/*
	//wrong parameters
	reduce_secret_to_1byte(NULL, 0, NULL);
	encode_uint8_uniform(NULL, NULL, 2, 1, 0, 1);
	encode_uint8_uniform(NULL, NULL, 1, 2, 0, 0);
	*/
	
	//normal runs
	uint8_t reduction, i,			//reduction result, cycle counter
	orig_array[5] = {20, 25, 30, 35, 40}, encoded_array[5], decoded_array[5],
	min = 15, max = 45, size = 5;	//minimum and maximum possible values in array, array size
	
	printf("Original array:\n");	//print it
	for (i = 0; i < 5; i++)
		printf("%i ", orig_array[i]);
	printf("\n\n");
	
	printf("if reduction = 0:\n");	//print an encoded and decoded arrays
	encode_uint8_uniform(orig_array, encoded_array, min, max, 0, size);
	for (i = 0; i < 5; i++)
		printf("%i ", encoded_array[i]);
	printf("\n");
	decode_uint8_uniform(encoded_array, decoded_array, min, max, 0, size);
	for (i = 0; i < 5; i++)
		printf("%i ", decoded_array[i]);
	printf("\n\n");
	
	reduce_secret_to_1byte(plaintext, strlen((char *)plaintext), &reduction);	
	printf("if reduction = %i:\n", reduction);	//print an encoded and decoded arrays
	encode_uint8_uniform(orig_array, encoded_array, min, max, reduction, size);
	for (i = 0; i < 5; i++)
		printf("%i ", encoded_array[i]);
	printf("\n");
	decode_uint8_uniform(encoded_array, decoded_array, min, max, reduction, size);
	for (i = 0; i < 5; i++)
		printf("%i ", decoded_array[i]);
	printf("\n\n");
	
	//encryption tests-----------------------------------------------------------------------------
	
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
	
	return 0;
}
