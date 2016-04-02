/*
test_uint8_uniform.c - test program for honeydata library
License: BSD 2-Clause
*/

#include <openssl/conf.h>

#include "test_common.h"

static int8_t print_uint8_array(const uint8_t *array, const uint64_t size)
{
	uint64_t i;
	
	//wrong input value
	if (size < 1) {
		error("size < 1");
		return 1;
		}
	
	for (i = 0; i < size; i++)
		printf("%i ", array[i]);
	printf("\n");
	return 0;
}

static int8_t print_uint16_array(const uint16_t *array, const uint64_t size)
{
	uint64_t i;
	
	//wrong input value
	if (size < 1) {
		error("size < 1");
		return 1;
		}
	
	for (i = 0; i < size; i++)
		printf("%i ", array[i]);
	printf("\n");
	return 0;
}

//get a number of occurences of different elements in array
static int8_t stats_uint8_array(const uint8_t *in_array, const uint64_t size, uint64_t *stats)
{
	uint64_t i;
	uint8_t elt;	//current processing element
	
	//wrong input value
	if (size < 1) {
		error("size < 1");
		return 1;
		}
	
	for (i = 0; i < size; i++) {
		elt = in_array[i];		//read a current element
		++stats[elt];			//increment the corresponding number in output array
		}
		
	return 0;
}

//get a number of occurences of different elements in array
static int8_t stats_uint16_array(const uint16_t *in_array, const uint64_t size, uint64_t *stats)
{
	uint64_t i;
	uint16_t elt;	//current processing element
	
	//wrong input value
	if (size < 1) {
		error("size < 1");
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
	
	uint32_t i, j, size, maxsize = 1048576;	//cycle counters, current and maximum array sizes (1MB)
	//statistics on pseudorandom and output arrays
	uint64_t in_stats[256] = {0}, out_stats[256] = {0}, long_stats[65536] = {0};
	uint8_t min, max, orig_array[maxsize], decoded_array[maxsize];	//minimum and maximim in array
	uint16_t encoded_array[maxsize];
	FILE *fp;						//file variable
	
	//variables for bruteforce test
	uint16_t bfkey;				//current iteration number
	unsigned char big_key[32];	//current key
	
	//initialise the crypto library
	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();
	OPENSSL_config(NULL);
	if (!RAND_status()) {
		error("PRNG hasn't been seeded with enough data");
    	return 1;
		}
	
	
	
	//random data encoding, encryption, decryption, decoding---------------------------------------
	
	size = 256;
	if (!RAND_bytes(orig_array, size))	//write a random numbers to original array
    	error_handler();

    //let orig_array contain numbers from 100 to 200
	for (i = 0; i < size; i++)
		orig_array[i] = (orig_array[i] % 101) + 100;

	encode_uint8_uniform(orig_array, encoded_array, 100, 200, size);
	ciphertext_len = encrypt((unsigned char *)encoded_array, 2*size, key, iv, ciphertext);
	decryptedtext_len = decrypt(ciphertext, ciphertext_len, key, iv, (unsigned char *)encoded_array);
	decode_uint8_uniform(encoded_array, decoded_array, 100, 200, decryptedtext_len);
	
	//compare result of decryption and original array
	if ( memcmp(orig_array, decoded_array, size) || (decryptedtext_len != 2*size) ) {
		error("orig_array and decoded_array are not the same");
		printf("size = %i, decryptedtext_len = %i\n", size, decryptedtext_len);
		print_uint8_array(orig_array, size);
		print_uint8_array(decoded_array, decryptedtext_len);
		return 1;
		}
	
	
	
	//bruteforce test (complexity equals 2^16) with statistics collection--------------------------
	
	size = 256;
	
	//encode and encrypt data
	encode_uint8_uniform(orig_array, encoded_array, 100, 200, size);    
	ciphertext_len = encrypt((unsigned char *)encoded_array, 2*size, key, iv, ciphertext);
	
	//let's try to bruteforce last 2 bytes of a key
	bfkey = 0;
	memcpy(big_key, key, 30);		//suppose that we know 30 first bytes of key
	memset(out_stats, 0, sizeof(out_stats));	//initialize statistics arrays
	
	for (i = 0; i < 65536; i++) {
		memcpy((void *)(big_key+30), &bfkey, sizeof(bfkey));	//get current key for decryption
		bfkey++;												//try next key on next iteration
		decryptedtext_len = decrypt(ciphertext, ciphertext_len, big_key, iv, (unsigned char *)encoded_array);
		decode_uint8_uniform(encoded_array, decoded_array, 100, 200, decryptedtext_len);
		if (decryptedtext_len != 2*size) {
			error("size and decryptedtext_len are not the equal");
			printf("size = %i, decryptedtext_len = %i\n", size, decryptedtext_len);
			return 1;
			}
		//get a statistics on current bruteforce iteration
	    stats_uint8_array(decoded_array, size, out_stats);
		/*
		char temp[4];								//buffer for temporary output
		for (j=0; j < 32; j++) {					//print current key
			snprintf(temp, 4, "%02x", big_key[j]);	//output format is HEX-code
			printf("%s", temp);
			if ( (j+1) % 4 == 0) printf(" ");		//place a space every 2 bytes
			}
		printf("\n");
	    */
		}
		
	//write overall bruteforce statistics to file
	//try to open file for writing
	if ((fp = fopen("uint8_bruteforce.ods", "w")) == NULL) {
		error("can't open file 'uint8_bruteforce.ods' for writing");
	    return 1;
		}
		
	//compare actual vs. ideal distributions of output array
	if (fprintf(fp, "=CHITEST(A102:A202;B102:B202)\n") < 0) {
		error("cannot write to 'uint8_bruteforce.ods' file");
		if (fclose(fp) == EOF)
			perror("test: fclose error");
		return 1;
		}
	//write two columns to file: actual and ideal distribution for CHITEST
	for (i = 0; i < 256; i++) {
		if ( (i < 100) || (i > 200) ) {
			if (fprintf(fp, "%llu\t%i\n", out_stats[i], 0) < 0) {
				error("cannot write to 'uint8_bruteforce.ods' file");
				if (fclose(fp) == EOF)
					perror("test: fclose error");
				return 1;
				}
			}
		else
			/*166 111 = 65 536 (number of keys in brutforce) * 256 (size of each decrypted text in
			elements) / 101 (number of possible array values from 100 to 200) = 16 777 216 (total
			amount of numbers) / 101 (their possible values) - expected result in out_stats*/
			if (fprintf(fp,  "%llu\t%i\n", out_stats[i], 166111) < 0) {
				error("cannot write to 'uint8_bruteforce.ods' file");
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
	if (!RAND_bytes(orig_array, size))				//write a random numbers to original array
    	error_handler();
    memset(in_stats, 0, sizeof(in_stats));			//initialize statistics arrays
    memset(out_stats, 0, sizeof(out_stats));
    stats_uint8_array(orig_array, size, in_stats);	//get a statistics on a pseudorandom numbers
      
	//let orig_array contain numbers from 100 to 219 distributed uniformly
	for (j = 0; j < size; j++) {
		//write a fresh random byte to this position until it will be between 0 and 239
		while (orig_array[j] > 239) {
			if (!RAND_bytes((unsigned char *)(orig_array+j), 1))
	    		error_handler();
			}
				
		orig_array[j] = (orig_array[j] % 120) + 100;
		}
	
	encode_uint8_uniform(orig_array, encoded_array, 100, 219, size);
	stats_uint8_array((uint8_t *)encoded_array, 2*size, out_stats);	//get a statistics on an encoded array
	decode_uint8_uniform(encoded_array, decoded_array, 100, 219, size);
	if (memcmp(orig_array, decoded_array, size)) {
		error("orig_array and decoded_array are not the same");
		return 1;
		}

	//write statistics to file
	//try to open file for writing
	if ((fp = fopen("uint8_encoding.ods", "w")) == NULL) {	
		error("can't open file 'uint8_encoding.ods' for writing");
	    return 1;
		}

	//compare pseudorandom vs. ideal, actual vs. ideal distributions
	if (fprintf(fp, "=CHITEST(A2:A257;B2:B257)\t\t=CHITEST(C2:C257;D2:D257)\n") < 0) {
		error("cannot write to 'uint8_encoding.ods' file");
		if (fclose(fp) == EOF)
			perror("test: fclose error");
		return 1;
		}
	//write four columns to file: pseudorandom and ideal, actual and ideal distributions for CHITEST
	for (i = 0; i < 256; i++) {
		if (fprintf(fp, "%llu\t%i\t%llu\t%i\n",	in_stats[i], size/256, out_stats[i], size/128) < 0) {
			error("cannot write to 'uint8_encoding.ods' file");
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
	
	
	
	//random data encoding with advanced statistics collection (slow!)-----------------------------
	
	size = 256;
	memset(long_stats, 0, sizeof(long_stats));	//initialize statistics array
	for (i = 0; i < 65536; i++) {
		if (!RAND_bytes(orig_array, size))		//write a random numbers to original array
    		error_handler();
    	
   		//let orig_array contain numbers from 100 to 199 distributed uniformly
		for (j = 0; j < size; j++) {
			//write a fresh random byte to this position until it will be between 0 and 199
			while (orig_array[j] > 199) {
				if (!RAND_bytes((unsigned char *)(orig_array+j), 1))
		    		error_handler();
				}
				
			orig_array[j] = (orig_array[j] % 100) + 100;
			}

		encode_uint8_uniform(orig_array, encoded_array, 100, 199, size);
		stats_uint16_array(encoded_array, size, long_stats);
		}
	
	//write overall statistics to file
	//try to open file for writing
	if ((fp = fopen("uint8_encoding2.ods", "w")) == NULL) {
		error("can't open file 'uint8_encoding2.ods' for writing");
	    return 1;
		}
		
	//compare actual vs. ideal distributions of output array
	if (fprintf(fp, "=CHITEST(A2:A65537;B2:B65537)\n") < 0) {
		error("cannot write to 'uint8_encoding2.ods' file");
		if (fclose(fp) == EOF)
			perror("test: fclose error");
		return 1;
		}
	//write two columns to file: actual and ideal distribution for CHITEST
	for (i = 0; i < 65536; i++) {
		/*256 = 65 536 (number of encodings) * 256 (size of each array for encoding) / 65536
		(number of possible array values from 0 to 65 535) = 16 777 216 (total amount of
		numbers) / 65536 (their possible values) - expected result in long_stats*/
		if (fprintf(fp,  "%llu\t%i\n", long_stats[i], 256) < 0) {
			error("cannot write to 'uint8_encoding2.ods' file");
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
		if (!RAND_bytes(orig_array, i))	//write a random numbers to original array
    		error_handler();
    	get_uint8_array_minmax(orig_array, i, &min, &max);
		encode_uint8_uniform(orig_array, encoded_array, min, max, i);
    	decode_uint8_uniform(encoded_array, decoded_array, min, max, i);
    	if (memcmp(orig_array, decoded_array, i)) {
    		error("orig_array and decoded_array are not the same");
			print_uint8_array(orig_array, i);
			print_uint8_array(decoded_array, i);
			return 1;
			}
		}
	
	
	
	//fixed general cases--------------------------------------------------------------------------
	
	size = 5;
	orig_array[0] = 20;
	orig_array[1] = 25;
	orig_array[2] = 30;
	orig_array[3] = 35;
	orig_array[4] = 40;
	
	printf("Original array:\n");	//print it
	print_uint8_array(orig_array, size);
	get_uint8_array_minmax(orig_array, size, &min, &max);
	
	printf("min = %i, max = %i:\n", min, max);
	encode_uint8_uniform(orig_array, encoded_array, min, max, size);
	print_uint16_array(encoded_array, size);
	decode_uint8_uniform(encoded_array, decoded_array, min, max, size);
	//if original and decoded arrays is not equal then print a decoded array too
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
	printf("\n");
	
	
	
	//fixed special cases--------------------------------------------------------------------------
	
	size = 5;
	orig_array[0] = 20;
	orig_array[1] = 20;
	orig_array[2] = 20;
	orig_array[3] = 20;
	orig_array[4] = 20;
	
	printf("Original array:\n");
	print_uint8_array(orig_array, size);
	get_uint8_array_minmax(orig_array, size, &min, &max);
	
	printf("min = %i, max = %i:\n", min, max);
	encode_uint8_uniform(orig_array, encoded_array, min, max, size);
	print_uint16_array(encoded_array, size);
	decode_uint8_uniform(encoded_array, decoded_array, min, max, size);
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
	get_uint8_array_minmax(NULL, 0, NULL, NULL);
	encode_uint8_uniform(NULL, NULL, 2, 1, 1);
	encode_uint8_uniform(NULL, NULL, 1, 2, 0);
	encode_uint8_uniform(orig_array, encoded_array, 100, 100, size);
	encode_uint8_uniform(orig_array, encoded_array, 21, 100, size);
	decode_uint8_uniform(NULL, NULL, 2, 1, 1);
	decode_uint8_uniform(NULL, NULL, 1, 2, 0);
	print_uint8_array(NULL, 0);
	print_uint16_array(NULL, 0);
	stats_uint8_array(NULL, 0, NULL);
	stats_uint16_array(NULL, 0, NULL);
	
	
	//clean up
	RAND_cleanup();
	EVP_cleanup();
	ERR_free_strings();
	
	//getchar();	//for debugging purposes
	
	return 0;
}
