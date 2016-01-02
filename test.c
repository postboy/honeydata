/*
test.c - test program for honeydata library
License: BSD 2-Clause
*/

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>

#include "lib/hdata.h"	//honeydata library

#define AES_BSIZE 128	//AES block size

static void error_handler(void)
{
	ERR_print_errors_fp(stderr);
	abort();
}

static int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv,
	unsigned char *ciphertext)
{
	EVP_CIPHER_CTX *ctx;

	int len;

	int ciphertext_len;

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

	int len;

	int plaintext_len;

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

	//a 256 bit key
	unsigned char *key = (unsigned char *)"01234567890123456789012345678901";

	//a 128 bit IV
	unsigned char *iv = (unsigned char *)"01234567890123456";

	//message to be encrypted
	unsigned char *plaintext = (unsigned char *)"The quick brown fox jumps over the lazy dog";

	/*Buffer for ciphertext. Ensure the buffer is long enough for the ciphertext which may be
	longer than the plaintext, dependant on the algorithm and mode*/
	unsigned char ciphertext[2*AES_BSIZE];

	//buffer for the decrypted text
	unsigned char decryptedtext[AES_BSIZE];

	int decryptedtext_len, ciphertext_len;

	//initialise the library
	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();
	OPENSSL_config(NULL);

	//encrypt the plaintext
	ciphertext_len = encrypt(plaintext, strlen ((char *)plaintext), key, iv, ciphertext);

	//do something useful with the ciphertext here
	printf("Ciphertext is:\n");
	BIO_dump_fp(stdout, (const char *)ciphertext, ciphertext_len);

	//decrypt the ciphertext
	decryptedtext_len = decrypt(ciphertext, ciphertext_len, key, iv, decryptedtext);

	//add a NULL terminator, because we are expecting printable text
	decryptedtext[decryptedtext_len] = '\0';

	//show the decrypted text
	printf("Decrypted text is:\n");
	printf("%s\n", decryptedtext);

	//clean up
	EVP_cleanup();
	ERR_free_strings();

	return 0;
}
