/*
int8_uniform_test.c - test program for honeydata library
License: BSD 2-Clause
*/

#include <openssl/conf.h>

#include "test_common.h"

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
	
	int32_t i, j;					//cycle counters					
	size_t size, maxsize = 1048576;	//current and maximum array sizes (1MB)
	//statistics on pseudorandom and output arrays
	uint64_t in_stats[256] = {0}, out_stats[256] = {0}, long_stats[65536] = {0};
	int8_t min, max, orig_array[maxsize], decoded_array[maxsize];	//minimum and maximim in array
	uint16_t encoded_array[maxsize];
	FILE *fp;						//file variable
	
	//variables for bruteforce test
	uint16_t bfkey;				//current iteration number
	unsigned char big_key[32];	//current key
	
	test_init();

	
	
	//random data encoding, encryption, decryption, decoding---------------------------------------
	
	size = 256;
	if (!RAND_bytes((unsigned char *)orig_array, size))	//write a random numbers to original array
    	OpenSSL_error();

    //let orig_array contain numbers from -50 to 50
	for (i = 0; i < size; i++)
		orig_array[i] = ((orig_array[i] - INT8_MIN) % 101) - 50;

	encode_int8_uniform(orig_array, encoded_array, size, -50, 50);
	ciphertext_len = encrypt((unsigned char *)encoded_array, 2*size, key, iv, ciphertext);
	decryptedtext_len = decrypt(ciphertext, ciphertext_len, key, iv,
								(unsigned char *)encoded_array);
	decode_int8_uniform(encoded_array, decoded_array, size, -50, 50);
	
	//compare result of decryption and original array
	if ( memcmp(orig_array, decoded_array, size) || (decryptedtext_len != 2*size) ) {
		error("orig_array and decoded_array are not the same");
		printf("size = %i, decryptedtext_len = %i\n", size, decryptedtext_len);
		print_int8_array(orig_array, size);
		print_int8_array(decoded_array, decryptedtext_len);
		test_error();
		}
	
	
	
	//bruteforce test (complexity equals 2^16) with statistics collection--------------------------
	
	size = 256;
	
	//encode and encrypt data
	encode_int8_uniform(orig_array, encoded_array, size, -50, 50);
	ciphertext_len = encrypt((unsigned char *)encoded_array, 2*size, key, iv, ciphertext);
	
	//let's try to bruteforce last 2 bytes of a key
	bfkey = 0;
	memcpy(big_key, key, 30);					//suppose that we know 30 first bytes of key
	memset(out_stats, 0, sizeof(out_stats));	//initialize statistics arrays
	
	for (i = 0; i < 65536; i++) {
		memcpy((void *)(big_key+30), &bfkey, sizeof(bfkey));	//get current key for decryption
		bfkey++;												//try next key on next iteration
		decryptedtext_len = decrypt(ciphertext, ciphertext_len, big_key, iv,
									(unsigned char *)encoded_array);
		decode_int8_uniform(encoded_array, decoded_array, size, -50, 50);
		if (decryptedtext_len != 2*size) {
			error("size and decryptedtext_len are not the equal");
			printf("size = %i, decryptedtext_len = %i\n", size, decryptedtext_len);
			test_error();
			}
		//get a statistics on current bruteforce iteration
	    stats_int8_array(decoded_array, size, out_stats);
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
	if ((fp = fopen("int8_bruteforce.ods", "w")) == NULL) {
		error("can't open file 'int8_bruteforce.ods' for writing");
	    test_error();
		}
		
	//compare actual vs. ideal distributions of output array
	if (fprintf(fp, "\t=CHITEST(B80:B180;C80:C180)\n") < 0) {
		error("cannot write to 'int8_bruteforce.ods' file");
		if (fclose(fp) == EOF)
			perror("test: fclose error");
		test_error();
		}
	//write two columns to file: actual and ideal distribution for CHITEST
	for (i = INT8_MIN; i <= INT8_MAX; i++) {
		if ( (i < -50) || (i > 50) ) {
			if (fprintf(fp, "%i\t%llu\t%i\n", i, out_stats[i-INT8_MIN], 0) < 0) {
				error("cannot write to 'int8_bruteforce.ods' file");
				if (fclose(fp) == EOF)
					perror("test: fclose error");
				test_error();
				}
			}
		else
			/*166 111 = 65 536 (number of keys in brutforce) * 256 (size of each decrypted text in
			elements) / 101 (number of possible array values from -50 to 50) = 16 777 216 (total
			amount of numbers) / 101 (their possible values) - expected result in out_stats*/
			if (fprintf(fp,  "%i\t%llu\t%i\n", i, out_stats[i-INT8_MIN], 166111) < 0) {
				error("cannot write to 'int8_bruteforce.ods' file");
				if (fclose(fp) == EOF)
					perror("test: fclose error");
				test_error();
				}
		}
	//close file
	if (fclose(fp) == EOF) {
		perror("test: fclose error");
		test_error();
		}
	
	
	
	//random data encoding and decoding with statistics collection---------------------------------

	size = maxsize;
	if (!RAND_bytes((unsigned char *)orig_array, size))	//write a random numbers to original array
    	OpenSSL_error();
    memset(in_stats, 0, sizeof(in_stats));				//initialize statistics arrays
    memset(out_stats, 0, sizeof(out_stats));
    stats_int8_array(orig_array, size, in_stats);		//get a statistics on a pseudorandom numbers
      
	//let orig_array contain numbers from -100 to 19 distributed uniformly
	for (j = 0; j < size; j++) {
		while ( (orig_array[j] < -100) || (orig_array[j] > 19) ) {
			if (!RAND_bytes((unsigned char *)(orig_array+j), 1))
	    		OpenSSL_error();
			}
		}
	
	encode_int8_uniform(orig_array, encoded_array, size, -100, 19);
	//get a statistics on an encoded array
	stats_int8_array((int8_t *)encoded_array, 2*size, out_stats);
	decode_int8_uniform(encoded_array, decoded_array, size, -100, 19);
	if (memcmp(orig_array, decoded_array, size)) {
		error("orig_array and decoded_array are not the same");
		test_error();
		}

	//write statistics to file
	//try to open file for writing
	if ((fp = fopen("int8_encoding.ods", "w")) == NULL) {	
		error("can't open file 'int8_encoding.ods' for writing");
	    test_error();
		}

	//compare pseudorandom vs. ideal, actual vs. ideal distributions
	if (fprintf(fp, "\t=CHITEST(B2:B257;C2:C257)\t\t=CHITEST(D2:D257;E2:E257)\n") < 0) {
		error("cannot write to 'int8_encoding.ods' file");
		if (fclose(fp) == EOF)
			perror("test: fclose error");
		test_error();
		}
	//write four columns to file: pseudorandom and ideal, actual and ideal distributions for CHITEST
	for (i = 0; i <= UINT8_MAX; i++) {
		if (fprintf(fp, "%i\t%llu\t%i\t%llu\t%i\n", i, in_stats[i], size/256, out_stats[i],
					size/128) < 0) {
			error("cannot write to 'int8_encoding.ods' file");
			if (fclose(fp) == EOF)
				perror("test: fclose error");
			test_error();
			}
		}
	//close file
	if (fclose(fp) == EOF) {
		perror("test: fclose error");
		test_error();
		}
	
	
	
	//random data encoding with advanced statistics collection (slow!)-----------------------------
	
	size = 256;
	memset(long_stats, 0, sizeof(long_stats));	//initialize statistics array
	for (i = 0; i < 65536; i++) {
		//write a random numbers to original array
		if (!RAND_bytes((unsigned char *)orig_array, size))
    		OpenSSL_error();
    	
   		//let orig_array contain numbers from -100 to -1 distributed uniformly
		for (j = 0; j < size; j++) {
			while ( (orig_array[j] < -100) || (orig_array[j] > -1) ) {
				if (!RAND_bytes((unsigned char *)(orig_array+j), 1))
		    		OpenSSL_error();
				}
			}

		encode_int8_uniform(orig_array, encoded_array, size, -100, -1);
		stats_uint16_array(encoded_array, size, long_stats);
		}
	
	//write overall statistics to file
	//try to open file for writing
	if ((fp = fopen("int8_encoding2.ods", "w")) == NULL) {
		error("can't open file 'int8_encoding2.ods' for writing");
	    test_error();
		}
		
	//compare actual vs. ideal distributions of output array
	if (fprintf(fp, "\t=CHITEST(B2:B65537;C2:C65537)\n") < 0) {
		error("cannot write to 'int8_encoding2.ods' file");
		if (fclose(fp) == EOF)
			perror("test: fclose error");
		test_error();
		}
	//write two columns to file: actual and ideal distribution for CHITEST
	for (i = 0; i <= UINT16_MAX; i++) {
		/*256 = 65 536 (number of encodings) * 256 (size of each array for encoding) / 65536
		(number of possible array values from 0 to 65 535) = 16 777 216 (total amount of
		numbers) / 65536 (their possible values) - expected result in long_stats*/
		if (fprintf(fp,  "%i\t%llu\t%i\n", i, long_stats[i], 256) < 0) {
			error("cannot write to 'int8_encoding2.ods' file");
			if (fclose(fp) == EOF)
				perror("test: fclose error");
			test_error();
			}
		}
	//close file
	if (fclose(fp) == EOF) {
		perror("test: fclose error");
		test_error();
		}
	
	
	
	//random encoding and decoding-----------------------------------------------------------------
	for (i = 1; i <= UINT8_MAX; i++) {
		//write a random numbers to original array
		if (!RAND_bytes((unsigned char *)orig_array, i))
    		OpenSSL_error();
    	get_int8_minmax(orig_array, i, &min, &max);
		encode_int8_uniform(orig_array, encoded_array, i, min, max);
    	decode_int8_uniform(encoded_array, decoded_array, i, min, max);
    	if (memcmp(orig_array, decoded_array, i)) {
    		error("orig_array and decoded_array are not the same");
			print_int8_array(orig_array, i);
			print_int8_array(decoded_array, i);
			test_error();
			}
		}
	
	
	
	//fixed general cases--------------------------------------------------------------------------
	
	size = 5;
	orig_array[0] = -20;
	orig_array[1] = -25;
	orig_array[2] = -30;
	orig_array[3] = -35;
	orig_array[4] = -40;
	
	printf("Original array:\n");	//print it
	print_int8_array(orig_array, size);
	get_int8_minmax(orig_array, size, &min, &max);
	
	printf("min = %i, max = %i:\n", min, max);
	encode_int8_uniform(orig_array, encoded_array, size, min, max);
	print_uint16_array(encoded_array, size);
	decode_int8_uniform(encoded_array, decoded_array, size, min, max);
	//if original and decoded arrays is not equal then print a decoded array too
	if (memcmp(orig_array, decoded_array, size))
		print_int8_array(decoded_array, size);
	
	printf("min = -45, max = -15:\n");
	encode_int8_uniform(orig_array, encoded_array, size, -45, -15);
	print_uint16_array(encoded_array, size);
	decode_int8_uniform(encoded_array, decoded_array, size, -45, -15);
	if (memcmp(orig_array, decoded_array, size))
		print_int8_array(decoded_array, size);
	
	printf("min = %i, max = %i:\n", INT8_MIN, INT8_MAX);
	encode_int8_uniform(orig_array, encoded_array, size, INT8_MIN, INT8_MAX);
	print_uint16_array(encoded_array, size);
	decode_int8_uniform(encoded_array, decoded_array, size, INT8_MIN, INT8_MAX);
	if (memcmp(orig_array, decoded_array, size))
		print_int8_array(decoded_array, size);
	printf("\n");
	
	
	
	//fixed special cases--------------------------------------------------------------------------
	
	size = 5;
	orig_array[0] = -20;
	orig_array[1] = -20;
	orig_array[2] = -20;
	orig_array[3] = -20;
	orig_array[4] = -20;
	
	printf("Original array:\n");
	print_int8_array(orig_array, size);
	get_int8_minmax(orig_array, size, &min, &max);
	
	printf("min = %i, max = %i:\n", min, max);
	encode_int8_uniform(orig_array, encoded_array, size, min, max);
	print_uint16_array(encoded_array, size);
	decode_int8_uniform(encoded_array, decoded_array, size, min, max);
	if (memcmp(orig_array, decoded_array, size))
		print_int8_array(decoded_array, size);
	
	printf("min = %i, max = %i:\n", INT8_MIN, INT8_MAX);
	encode_int8_uniform(orig_array, encoded_array, size, INT8_MIN, INT8_MAX);
	print_uint16_array(encoded_array, size);
	decode_int8_uniform(encoded_array, decoded_array, size, INT8_MIN, INT8_MAX);
	if (memcmp(orig_array, decoded_array, size))
		print_int8_array(decoded_array, size);
	printf("\n");
	
	
	
	//wrong parameters-----------------------------------------------------------------------------
	get_int8_minmax(NULL, 0, NULL, NULL);
	get_int8_minmax(orig_array, 0, NULL, NULL);
	get_int8_minmax(orig_array, 1, NULL, NULL);
	get_int8_minmax(orig_array, 1, &min, NULL);
	printf("\n");
	
	encode_int8_uniform(NULL, NULL, 0, 0, 0);
	encode_int8_uniform(orig_array, NULL, 0, 0, 0);
	encode_int8_uniform(orig_array, encoded_array, 0, 0, 0);
	encode_int8_uniform(orig_array, encoded_array, 1, -2, -100);
	encode_int8_uniform(orig_array, encoded_array, size, -19, 0);
	encode_int8_uniform(orig_array, encoded_array, size, -100, -21);
	printf("\n");
	
	decode_int8_uniform(NULL, NULL, 0, 0, 0);
	decode_int8_uniform(encoded_array, NULL, 0, 0, 0);
	decode_int8_uniform(encoded_array, orig_array, 0, 0, 0);
	decode_int8_uniform(encoded_array, orig_array, 1, -2, -100);
	printf("\n");
	
	stats_int8_array(NULL, 0, NULL);
	stats_int8_array(orig_array, 0, NULL);
	stats_int8_array(orig_array, 1, NULL);
	printf("\n");
	
	print_int8_array(NULL, 0);
	print_int8_array(orig_array, 0);
	
	
	
	test_deinit();

	return 0;
	
}
