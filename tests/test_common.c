/*
test_common.c - common code for test programs for honeydata library
License: BSD 2-Clause
*/

#include "test_common.h"

//encrypt a message
extern int encrypt(const unsigned char *plaintext, const size_t plaintext_len,
	const unsigned char *key, const unsigned char *iv, unsigned char *ciphertext)
{
	EVP_CIPHER_CTX *ctx;

	int len, ciphertext_len;

	//create and initialise the context
	if (!(ctx = EVP_CIPHER_CTX_new()))
		OpenSSL_error();
	//EVP_CIPHER_CTX_set_padding(ctx, 0);
	
	/*Initialise the encryption operation. IMPORTANT - ensure you use a key and IV size
	appropriate for your cipher. In this example we are using 256 bit AES (i.e. a 256 bit key). The
	IV size for *most* modes is the same as the block size. For AES this is 128 bits.*/
	if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1)
		OpenSSL_error();
	EVP_CIPHER_CTX_set_padding(ctx, 0);
	/*We should disable the padding for plausible decryption with any decryption key. The total
	amount of data encrypted or decrypted must then be a multiple of the block size or an error
	will occur.*/

	/*Provide the message to be encrypted, and obtain the encrypted output. EVP_EncryptUpdate can
	be called multiple times if necessary.*/
	if (EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len) != 1)
		OpenSSL_error();
	ciphertext_len = len;

	//Finalise the encryption. Further ciphertext bytes may be written at this stage.
	if (EVP_EncryptFinal_ex(ctx, ciphertext + len, &len) != 1)
		OpenSSL_error();
  	ciphertext_len += len;

	//clean up
	EVP_CIPHER_CTX_free(ctx);

	return ciphertext_len;
}

//decrypt a message
extern int decrypt(const unsigned char *ciphertext, const size_t ciphertext_len,
	const unsigned char *key, const unsigned char *iv, unsigned char *plaintext)
{
	EVP_CIPHER_CTX *ctx;

	int len, plaintext_len;

	//create and initialise the context
	if (!(ctx = EVP_CIPHER_CTX_new()))
		OpenSSL_error();
	
	/*Initialise the decryption operation. IMPORTANT - ensure you use a key and IV size appropriate
	for your cipher. In this example we are using 256 bit AES (i.e. a 256 bit key). The IV size for
	*most* modes is the same as the block size. For AES this is 128 bits.*/
	if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1)
		OpenSSL_error();
	EVP_CIPHER_CTX_set_padding(ctx, 0);	//disable padding

	/*Provide the message to be decrypted, and obtain the plaintext output. EVP_DecryptUpdate can
	be called multiple times if necessary.*/
	if (EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len) != 1)
		OpenSSL_error();
	plaintext_len = len;

	//Finalise the decryption. Further plaintext bytes may be written at this stage.
	if (EVP_DecryptFinal_ex(ctx, plaintext + len, &len) != 1)
		OpenSSL_error();
	plaintext_len += len;

	//clean up
	EVP_CIPHER_CTX_free(ctx);

	return plaintext_len;
}

//parameters in following generic functions:
//itype - type of input elements

//generic function for getting a number of occurences of different elements in integer array
#define STATS_INT_ARRAY(itype, min) \
(const itype *in_array, const size_t size, uint64_t *stats) \
{ \
	/*check the arguments*/ \
	if (in_array == NULL) { \
		error("in_array = NULL"); \
		return 1; \
		} \
	if (size == 0) { \
		error("size = 0"); \
		return 2; \
		} \
	if (stats == NULL) { \
		error("stats = NULL"); \
		return 3; \
		} \
	\
	size_t i; \
	itype elt;	/*current processing element*/ \
	\
	for (i = 0; i < size; i++) { \
		elt = in_array[i];	/*read a current element*/ \
		++stats[elt - min];	/*increment the corresponding number in output array*/ \
		} \
	\
	return 0; \
}

extern int8_t stats_uint8_array
	STATS_INT_ARRAY(uint8_t, 0)

extern int8_t stats_int8_array
	STATS_INT_ARRAY(int8_t, INT8_MIN)

extern int8_t stats_uint16_array
	STATS_INT_ARRAY(uint16_t, 0)

#undef STATS_INT_ARRAY

//generic function for printing a numeric array
#define PRINT_ARRAY(itype, format) \
(const itype *array, const size_t size) \
{ \
	/*check the arguments*/ \
	if (array == NULL) { \
		error("array = NULL"); \
		return 1; \
		} \
	if (size == 0) { \
		error("size = 0"); \
		return 2; \
		} \
	\
	size_t i; \
	\
	for (i = 0; i < size; i++) \
		printf( (format), array[i]); \
	printf("\n"); \
	return 0; \
}

extern int8_t print_uint8_array
	PRINT_ARRAY(uint8_t, "%"PRIu8" ")

extern int8_t print_int8_array
	PRINT_ARRAY(int8_t, "%"PRIi8" ")

extern int8_t print_uint16_array
	PRINT_ARRAY(uint16_t, "%"PRIu16" ")

#undef PRINT_ARRAY

extern void test_init(void)
{
	//initialise the crypto library
	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();
	//OPENSSL_config(NULL);
	if (!RAND_status()) {
		error("PRNG hasn't been seeded with enough data");
    	exit(1);
		}
}

extern void test_deinit(void)
{
	//clean up
	RAND_cleanup();
	EVP_cleanup();
	ERR_free_strings();
}

extern void OpenSSL_error(void)
{
	ERR_print_errors_fp(stderr);
	test_error();
}

extern void test_error(void)
{
	test_deinit();
	exit(1);
}
