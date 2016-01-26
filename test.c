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
	//EVP_CIPHER_CTX_set_padding(ctx, 0);
	
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
	
	/*Buffer for ciphertext. Ensure the buffer is long enough for the ciphertext which may be
	longer than the plaintext, dependant on the algorithm and mode (for AES-256 in CBC mode we need
	one extra block)*/
	unsigned char ciphertext[512], ciphertext_save[512];
	uint32_t decryptedtext_len, ciphertext_len;	//their lengths
	
	uint32_t i, j, size, maxsize = 2097152;	//cycle counters, current and maximum array sizes (2MB)
	//statistics on pseudorandom and output arrays
	uint64_t in_stats[256] = {0}, out_stats[256] = {0};
	uint8_t reduction, min, max,	//reduction result, minimum and maximim
	orig_array[maxsize], encoded_array[maxsize], decoded_array[maxsize];
	FILE *fp;						//file variable
	
	//initialise the crypto library
	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();
	OPENSSL_config(NULL);
	if (!RAND_status()) {
		fprintf(stderr, "test: RAND_status error: PRNG hasn't been seeded with enough data\n");
    	return 1;
		}
	
	reduce_secret_to_1byte(key, 32, &reduction);	//get a reduction for honey encryption
	
	//random data encoding, encryption, decryption, decoding---------------------------------------
	size = 256;
	if (!RAND_bytes(orig_array, size)) {			//write a random numbers to original array
    	ERR_print_errors_fp(stderr);
    	return 1;
    	}

    //let orig_array contain numbers from 100 to 250
	for (i = 0; i < size; i++)
		orig_array[i] = (orig_array[i] % 151) + 100;

	encode_uint8_uniform(orig_array, encoded_array, 100, 250, reduction, size);    
	ciphertext_len = encrypt(encoded_array, size, key, iv, ciphertext);
	memcpy(ciphertext_save, ciphertext, sizeof(ciphertext));
	decryptedtext_len = decrypt(ciphertext, ciphertext_len, key, iv, encoded_array);
	decode_uint8_uniform(encoded_array, decoded_array, 100, 250, reduction, decryptedtext_len);
	
	//compare result of decryption and original array
	if ( memcmp(orig_array, decoded_array, size) || (decryptedtext_len != size) ) {
		fprintf(stderr, "test: memcmp error: orig_array and decoded_array are not the same.\n");
		printf("size = %i, decryptedtext_len = %i\n", size, decryptedtext_len);
		print_uint8_array(orig_array, size);
		print_uint8_array(decoded_array, decryptedtext_len);
		return 1;
		}
		
	//bruteforce test (complexity equals 2^16) with statistics collection--------------------------
	uint16_t bfkey;
	unsigned char big_key[32];
	char temp[4];	//buffer for temporary output
	
	//let's try to bruteforce last 2 bytes of a key
	memcpy(big_key, key, 30);		//suppose that we know 30 first bytes of key
	memset(out_stats, 0, 8*256);	//initialize statistics array
	for (i = 0; i < 65536; i++) {
		memcpy((void *)(big_key+30), &bfkey, sizeof(bfkey));	//get current key for decryption
		bfkey++;									//try next key on next iteration
		memcpy(ciphertext, ciphertext_save, sizeof(ciphertext_save));
		decryptedtext_len = decrypt(ciphertext, ciphertext_len, big_key, iv, encoded_array);
		decode_uint8_uniform(encoded_array, decoded_array, 100, 250, reduction, decryptedtext_len);
		if (decryptedtext_len != size) {
			fprintf(stderr, "test: error: size and decryptedtext_len are not the equal.\n");
			printf("size = %i, decryptedtext_len = %i\n", size, decryptedtext_len);
			return 1;
			}
		//get a statistics on current bruteforce iteration
	    array_statistics(decoded_array, decryptedtext_len, out_stats);
		/*
		for (j=0; j < 32; j++) {					//print current key
			snprintf(temp, 4, "%02x", big_key[j]);	//output format is HEX-code
			printf("%s", temp);
			if ( (j+1) % 4 == 0) printf(" ");		//place a space every 2 bytes
			}
	    printf("\n");
	    */
		}
		
	//write overall bruteforce statistics to file
	//try to open file 'uint8_bruteforce.ods' for writing
	if ((fp = fopen("uint8_bruteforce.ods", "w")) == NULL) {
		fprintf(stderr, "test: fopen error: can't open file 'uint8_bruteforce.ods' for writing.\n");
	    return 1;
		}
	//write two columns to file: actual and ideal distributions for CHITEST
	//compare actual vs. ideal distributions
	for (i = 0; i < 256; i++) {
		if (i == 0) {
			if (fprintf(fp, "%llu\t%i\t=CHITEST(A1:A256;C1:C256)\n", out_stats[i], size) < 0) {
				fprintf(stderr, "test: fwrite error: cannot write to 'uint8_bruteforce.ods' file.\n");
				if (fclose(fp) == EOF)
					perror("test: fclose error");
				return 1;
				}
			}
		else
			if (fprintf(fp, "%llu\t%i\n", out_stats[i], size) < 0) {
				fprintf(stderr, "test: fwrite error: cannot write to 'uint8_bruteforce.ods' file.\n");
				if (fclose(fp) == EOF)
					perror("test: fclose error");
				return 1;
				}
		}
	//close file
	if (fclose(fp) == EOF) {
		perror("test: fclose error");
		return 1;
		}
	
	/*
	//random data encoding and decoding with statistics collection---------------------------------
	size = maxsize;
	if (!RAND_bytes(orig_array, size)) {			//write a random numbers to original array
    	ERR_print_errors_fp(stderr);
    	return 1;
    	}
    memset(in_stats, 0, 8*256);						//initialize statistics array
    array_statistics(orig_array, size, in_stats);	//get a statistics on a pseudorandom numbers
      
    //let orig_array contain numbers from 120 to 239
	for (i = 0; i < size; i++)
		orig_array[i] = (orig_array[i] % 120) + 120;
	encode_uint8_uniform(orig_array, encoded_array, 120, 239, reduction, size);
	memset(out_stats, 0, 8*256);						//initialize statistics array
	array_statistics(encoded_array, size, out_stats);	//get a statistics on an encoded array
	decode_uint8_uniform(encoded_array, decoded_array, 120, 239, reduction, size);
	if (memcmp(orig_array, decoded_array, size)) {
		fprintf(stderr, "test: memcmp error: orig_array and decoded_array are not the same.\n");
		return 1;
		}

	//write statistics to file
	if ((fp = fopen("uint8_encoding.ods", "w")) == NULL) {	//try to open file 'uint8_encoding.ods' for writing
		fprintf(stderr, "test: fopen error: can't open file 'uint8_encoding.ods' for writing.\n");
	    return 1;
		}
	//write three columns to file: pseudorandom, actual and ideal distributions for CHITEST
	//compare pseudorandom vs. ideal, actual vs. ideal distributions
	for (i = 0; i < 256; i++) {
		if (i == 0) {
			if (fprintf(fp, "%llu\t%llu\t%i\t=CHITEST(A1:A256;C1:C256)\t=CHITEST(B1:B256;C1:C256)\n",
					in_stats[i], out_stats[i], size/256) < 0) {
				fprintf(stderr, "test: fwrite error: cannot write to 'uint8_encoding.ods' file.\n");
				if (fclose(fp) == EOF)
					perror("test: fclose error");
				return 1;
				}
			}
		else
			if (fprintf(fp, "%llu\t%llu\t%i\n", in_stats[i], out_stats[i], size/256) < 0) {
				fprintf(stderr, "test: fwrite error: cannot write to 'uint8_encoding.ods' file.\n");
				if (fclose(fp) == EOF)
					perror("test: fclose error");
				return 1;
				}
		}
	//close file
	if (fclose(fp) == EOF) {
		perror("test: fclose error");
		return 1;
		}
	*/	
		
	/*
	//random encoding and decoding-----------------------------------------------------------------
	for (i = 1; i < 256; i++) {
		if (!RAND_bytes(orig_array, i)) {	//write a random numbers to original array
    		ERR_print_errors_fp(stderr);
    		return 1;
    		}
    	get_uint8_array_metadata(orig_array, i, &min, &max);
		encode_uint8_uniform(orig_array, encoded_array, min, max, i, i);
    	decode_uint8_uniform(encoded_array, decoded_array, min, max, i, i);
    	if (memcmp(orig_array, decoded_array, i)) {
    		fprintf(stderr, "test: memcmp error: orig_array and decoded_array are not the same.\n");
			print_uint8_array(orig_array, i);
			print_uint8_array(decoded_array, i);
			return 1;
			}
		}
	*/
	
	/*
	//fixed general cases--------------------------------------------------------------------------
	orig_array[0] = 20;
	orig_array[1] = 25;
	orig_array[2] = 30;
	orig_array[3] = 35;
	orig_array[4] = 40;
	size = 5;
	
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
	//fixed special cases--------------------------------------------------------------------------
	orig_array[0] = 20;
	orig_array[1] = 20;
	orig_array[2] = 20;
	orig_array[3] = 20;
	orig_array[4] = 20;
	size = 5;
	
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
	//wrong parameters-----------------------------------------------------------------------------
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
	*/
	
	//clean up
	RAND_cleanup();
	EVP_cleanup();
	ERR_free_strings();
	
	//getchar();	//for debugging purposes
	
	return 0;
}
