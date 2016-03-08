/*
test_uint8_uniform.c - test program for honeydata library
License: BSD 2-Clause
*/

#include <openssl/conf.h>

#include "test_common.h"

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

static int8_t print_uint16_array(const uint16_t *array, const uint64_t size)
{
	uint64_t i;	//cycle counter
	
	//wrong input value
	if (size < 1) {
		fprintf(stderr, "test: print_uint16_array error: size < 1\n");
		return 1;
		}
	
	for (i = 0; i < size; i++)
		printf("%i ", array[i]);
	printf("\n");
	return 0;
}

//get a number of occurences of different bytes in array
static int8_t stats_uint8_array(const uint8_t *in_array, const uint64_t size, uint64_t *stats)
{
	uint64_t i;			//cycle counter
	unsigned char elt;	//current processing element
	
	//wrong input value
	if (size < 1) {
		fprintf(stderr, "test: stats_uint8_array error: size < 1\n");
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
	unsigned char ciphertext[1024];
	uint32_t decryptedtext_len, ciphertext_len;	//their lengths
	
	uint32_t i, j, size, maxsize = 2097152;	//cycle counters, current and maximum array sizes (2MB)
	//statistics on pseudorandom and output arrays
	uint64_t in_stats[256] = {0}, out_stats[256] = {0};
	uint8_t min, max, orig_array[maxsize], decoded_array[maxsize];	//minimum and maximim in array
	uint16_t encoded_array[maxsize];
	FILE *fp;						//file variable
	
	//variables for bruteforce test
	uint16_t bfkey;				//current iteration number
	unsigned char big_key[32];	//current key
	char temp[4];				//buffer for temporary output
	
	//initialise the crypto library
	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();
	OPENSSL_config(NULL);
	if (!RAND_status()) {
		fprintf(stderr, "test: RAND_status error: PRNG hasn't been seeded with enough data\n");
    	return 1;
		}
	
	
	
	//random data encoding, encryption, decryption, decoding---------------------------------------
	size = 256;
	if (!RAND_bytes(orig_array, size)) {			//write a random numbers to original array
    	ERR_print_errors_fp(stderr);
    	return 1;
    	}

    //let orig_array contain numbers from 100 to 200
	for (i = 0; i < size; i++)
		orig_array[i] = (orig_array[i] % 101) + 100;

	encode_uint8_uniform(orig_array, encoded_array, 100, 200, size);
	ciphertext_len = encrypt((unsigned char *)encoded_array, 2*size, key, iv, ciphertext);
	decryptedtext_len = decrypt(ciphertext, ciphertext_len, key, iv, (unsigned char *)encoded_array);
	decode_uint8_uniform(encoded_array, decoded_array, 100, 200, decryptedtext_len);
	
	//compare result of decryption and original array
	if ( memcmp(orig_array, decoded_array, size) || (decryptedtext_len != size) ) {
		fprintf(stderr, "test: memcmp error: orig_array and decoded_array are not the same.\n");
		printf("size = %i, decryptedtext_len = %i\n", size, decryptedtext_len);
		print_uint8_array(orig_array, size);
		print_uint8_array(decoded_array, decryptedtext_len);
		return 1;
		}
	
	
	
	//bruteforce test (complexity equals 2^16) with statistics collection--------------------------

	//encode and encrypt data
	encode_uint8_uniform(orig_array, encoded_array, 100, 200, size);    
	ciphertext_len = encrypt((unsigned char *)encoded_array, 2*size, key, iv, ciphertext);
	
	//let's try to bruteforce last 2 bytes of a key
	bfkey = 0;
	memcpy(big_key, key, 30);		//suppose that we know 30 first bytes of key
	memset(out_stats, 0, sizeof(out_stats));	//initialize statistics array
	for (i = 0; i < 65536; i++) {
		memcpy((void *)(big_key+30), &bfkey, sizeof(bfkey));	//get current key for decryption
		bfkey++;												//try next key on next iteration
		decryptedtext_len = decrypt(ciphertext, ciphertext_len, big_key, iv, (unsigned char *)encoded_array);
		decode_uint8_uniform(encoded_array, decoded_array, 100, 200, decryptedtext_len);
		if (decryptedtext_len != size) {
			fprintf(stderr, "test: error: size and decryptedtext_len are not the equal.\n");
			printf("size = %i, decryptedtext_len = %i\n", size, decryptedtext_len);
			return 1;
			}
		//get a statistics on current bruteforce iteration
	    stats_uint8_array(decoded_array, decryptedtext_len, out_stats);
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
		
	//compare actual vs. ideal distributions of output array
	if (fprintf(fp, "=CHITEST(A102:A202;B102:B202)\n") < 0) {
		fprintf(stderr, "test: fwrite error: cannot write to 'uint8_bruteforce.ods' file.\n");
		if (fclose(fp) == EOF)
			perror("test: fclose error");
		return 1;
		}
	//write two columns to file: actual and ideal distribution for CHITEST
	for (i = 0; i < 256; i++) {
		if ( (i < 100) || (i > 200) ) {
			if (fprintf(fp, "%llu\t%i\n", out_stats[i], 0) < 0) {
				fprintf(stderr, "test: fwrite error: cannot write to 'uint8_bruteforce.ods' file.\n");
				if (fclose(fp) == EOF)
					perror("test: fclose error");
				return 1;
				}
			}
		else
			/*111 107 = 65 536 (number of keys in brutforce) * 256 (size of each decrypted text in
			elements) / 101 (number of possible array values from 100 to 200) = 16 777 216 (total
			amount of numbers) / 101 (their possible values) - expected result in out_stats*/
			if (fprintf(fp,  "%llu\t%i\n", out_stats[i], 166111) < 0) {
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
	
	
	
	//random data encoding and decoding with statistics collection---------------------------------
	size = maxsize;
	if (!RAND_bytes(orig_array, size)) {			//write a random numbers to original array
    	ERR_print_errors_fp(stderr);
    	return 1;
    	}
    memset(in_stats, 0, sizeof(in_stats));						//initialize statistics array
    stats_uint8_array(orig_array, size, in_stats);	//get a statistics on a pseudorandom numbers
      
    //let orig_array contain numbers from 120 to 239
	for (i = 0; i < size; i++)
		orig_array[i] = (orig_array[i] % 120) + 120;
	encode_uint8_uniform(orig_array, encoded_array, 120, 239, size);
	memset(out_stats, 0, sizeof(out_stats));						//initialize statistics array
	stats_uint8_array((uint8_t *)encoded_array, 2*size, out_stats);	//get a statistics on an encoded array
	decode_uint8_uniform(encoded_array, decoded_array, 120, 239, size);
	if (memcmp(orig_array, decoded_array, size)) {
		fprintf(stderr, "test: memcmp error: orig_array and decoded_array are not the same.\n");
		return 1;
		}

	//write statistics to file
	if ((fp = fopen("uint8_encoding.ods", "w")) == NULL) {	//try to open file 'uint8_encoding.ods' for writing
		fprintf(stderr, "test: fopen error: can't open file 'uint8_encoding.ods' for writing.\n");
	    return 1;
		}

	//compare pseudorandom vs. ideal, actual vs. ideal distributions
	if (fprintf(fp, "=CHITEST(A2:A257;C2:C257)\t=CHITEST(B2:B257;C2:C257)\n") < 0) {
		fprintf(stderr, "test: fwrite error: cannot write to 'uint8_encoding.ods' file.\n");
		if (fclose(fp) == EOF)
			perror("test: fclose error");
		return 1;
		}
	//write three columns to file: pseudorandom, actual and ideal distributions for CHITEST
	for (i = 0; i < 256; i++) {
		if (fprintf(fp, "%llu\t%llu\t%i\n",	in_stats[i], out_stats[i], size/256) < 0) {
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
	
		
	
	//random encoding and decoding-----------------------------------------------------------------
	for (i = 1; i < 256; i++) {
		if (!RAND_bytes(orig_array, i)) {	//write a random numbers to original array
    		ERR_print_errors_fp(stderr);
    		return 1;
    		}
    	get_uint8_array_metadata(orig_array, i, &min, &max);
		encode_uint8_uniform(orig_array, encoded_array, min, max, i);
    	decode_uint8_uniform(encoded_array, decoded_array, min, max, i);
    	if (memcmp(orig_array, decoded_array, i)) {
    		fprintf(stderr, "test: memcmp error: orig_array and decoded_array are not the same.\n");
			print_uint8_array(orig_array, i);
			print_uint8_array(decoded_array, i);
			return 1;
			}
		}
	
	
	
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
	
	printf("min = %i, max = %i:\n", min, max);
	encode_uint8_uniform(orig_array, encoded_array, min, max, size);
	print_uint16_array(encoded_array, size);
	decode_uint8_uniform(encoded_array, decoded_array, min, max, size);
	if (memcmp(orig_array, decoded_array, size))
		print_uint8_array(decoded_array, size);
	
	printf("min = 15, max = 45:\n");
	encode_uint8_uniform(orig_array, encoded_array, 15, 45, size);
	print_uint16_array(encoded_array, size);
	decode_uint8_uniform(encoded_array, decoded_array, 15, 45, size);
	if (memcmp(orig_array, decoded_array, size))
		print_uint8_array(decoded_array, size);
	
	printf("min = 0, max = 255:\n");
	encode_uint8_uniform(orig_array, encoded_array, 0, 255, size);
	print_uint16_array(encoded_array, size);
	decode_uint8_uniform(encoded_array, decoded_array, 0, 255, size);
	if (memcmp(orig_array, decoded_array, size))
		print_uint8_array(decoded_array, size);
	
	
	
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
	
	printf("min = %i, max = %i:\n", min, max);
	encode_uint8_uniform(orig_array, encoded_array, min, max, size);
	print_uint16_array(encoded_array, size);
	decode_uint8_uniform(encoded_array, decoded_array, min, max, size);
	//if original and decoded arrays is not equal then print a decoded array too
	if (memcmp(orig_array, decoded_array, size))
		print_uint8_array(decoded_array, size);
	
	printf("min = 0, max = 255:\n");
	encode_uint8_uniform(orig_array, encoded_array, 0, 255, size);
	print_uint16_array(encoded_array, size);
	decode_uint8_uniform(encoded_array, decoded_array, 0, 255, size);
	if (memcmp(orig_array, decoded_array, size))
		print_uint8_array(decoded_array, size);
	printf("\n");
	
	
	
	//wrong parameters-----------------------------------------------------------------------------
	get_uint8_array_metadata(NULL, 0, NULL, NULL);
	encode_uint8_uniform(NULL, NULL, 2, 1, 1);
	encode_uint8_uniform(NULL, NULL, 1, 2, 0);
	encode_uint8_uniform(orig_array, encoded_array, 100, 100, size);
	encode_uint8_uniform(orig_array, encoded_array, 21, 100, size);
	decode_uint8_uniform(NULL, NULL, 2, 1, 1);
	decode_uint8_uniform(NULL, NULL, 1, 2, 0);
	print_uint8_array(NULL, 0);
	stats_uint8_array(NULL, 0, NULL);
	
	
	//clean up
	RAND_cleanup();
	EVP_cleanup();
	ERR_free_strings();
	
	//getchar();	//for debugging purposes
	
	return 0;
}
