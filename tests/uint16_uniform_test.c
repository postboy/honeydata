/*
uint16_uniform_test.c - test program for honeydata library
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
	size_t decryptedtext_len, ciphertext_len;	//their lengths
	
	int32_t i, j;								//cycle counters					
	size_t size, maxsize = 524288;				//current and maximum array sizes (1MB)
	#define BYTESIZE (size*sizeof(uint16_t))	//current input array size in bytes
	//statistics on pseudorandom and output arrays
	uint64_t in_stats[256] = {0}, out_stats[256] = {0}, long_stats[65536] = {0};
	uint16_t min, max, orig_array[maxsize], decoded_array[maxsize];	//minimum and maximim in array
	uint32_t encoded_array[maxsize];
	FILE *fp;						//file variable
	
	//variables for bruteforce test
	uint16_t bfkey;				//current iteration number
	unsigned char big_key[32];	//current key
	
	test_init();
	
	
	
	//random data encoding, encryption, decryption, decoding---------------------------------------
	
	size = 256;
	//write a random numbers to original array
	if (!RAND_bytes((unsigned char *)orig_array, BYTESIZE))
    	OpenSSL_error();
	
    //let orig_array contain numbers from 1000 to 2000
	for (i = 0; i < size; i++)
		orig_array[i] = (orig_array[i] % 1001) + 1000;
	
	get_uint16_minmax(orig_array, size, &min, &max);
	encode_uint16_uniform(orig_array, encoded_array, size, min, max);
	ciphertext_len = encrypt((unsigned char *)encoded_array, 2*BYTESIZE, key, iv, ciphertext);
	decryptedtext_len = decrypt(ciphertext, ciphertext_len, key, iv, (unsigned char *)encoded_array);
	decode_uint16_uniform(encoded_array, decoded_array, size, min, max);
	
	//compare result of decryption and original array
	if ( memcmp(orig_array, decoded_array, size) || (decryptedtext_len != 2*BYTESIZE) ) {
		error("orig_array and decoded_array are not the same");
		printf("size = %i, decryptedtext_len = %i\n", size, decryptedtext_len);
		print_uint16_array(orig_array, size);
		print_uint16_array(decoded_array, size);
		test_error();
		}
	
	
	/*
	//bruteforce test (complexity equals 2^16) with statistics collection--------------------------
	
	size = 256;
	
	//encode and encrypt data
	encode_uint16_uniform(orig_array, encoded_array, size, 100, 200);
	ciphertext_len = encrypt((unsigned char *)encoded_array, 2*BYTESIZE, key, iv, ciphertext);
	
	//let's try to bruteforce last 2 bytes of a key
	bfkey = 0;
	memcpy(big_key, key, 30);					//suppose that we know 30 first bytes of key
	memset(out_stats, 0, sizeof(out_stats));	//initialize statistics arrays
	
	for (i = 0; i < 65536; i++) {
		memcpy((void *)(big_key+30), &bfkey, sizeof(bfkey));	//get current key for decryption
		bfkey++;												//try next key on next iteration
		decryptedtext_len = decrypt(ciphertext, ciphertext_len, big_key, iv,
									(unsigned char *)encoded_array);
		decode_uint16_uniform(encoded_array, decoded_array, size, 100, 200);
		if (decryptedtext_len != 2*BYTESIZE) {
			error("size and decryptedtext_len are not the equal");
			printf("size = %i, decryptedtext_len = %i\n", size, decryptedtext_len);
			test_error();
			}
		//get a statistics on current bruteforce iteration
	    stats_uint8_array(decoded_array, size, out_stats);*/
		/*
		char temp[4];								//buffer for temporary output
		for (j=0; j < 32; j++) {					//print current key
			snprintf(temp, 4, "%02x", big_key[j]);	//output format is HEX-code
			printf("%s", temp);
			if ( (j+1) % 4 == 0) printf(" ");		//place a space every 2 bytes
			}
		printf("\n");
	    *//*
		}
		
	//write overall bruteforce statistics to file
	//try to open file for writing
	if ((fp = fopen("uint16_bruteforce.ods", "w")) == NULL) {
		error("can't open file 'uint16_bruteforce.ods' for writing");
		test_error();
		}
		
	//compare actual vs. ideal distributions of output array
	if (fprintf(fp, "\t=CHITEST(B102:B202;C102:C202)\n") < 0) {
		error("cannot write to 'uint16_bruteforce.ods' file");
		if (fclose(fp) == EOF)
			perror("test: fclose error");
		test_error();
		}
	//write two columns to file: actual and ideal distribution for CHITEST
	for (i = 0; i <= UINT8_MAX; i++) {
		if ( (i < 100) || (i > 200) ) {
			if (fprintf(fp, "%i\t%llu\t%i\n", i, out_stats[i], 0) < 0) {
				error("cannot write to 'uint16_bruteforce.ods' file");
				if (fclose(fp) == EOF)
					perror("test: fclose error");
				test_error();
				}
			}
		else*/
			/*166 111 = 65 536 (number of keys in brutforce) * 256 (size of each decrypted text in
			elements) / 101 (number of possible array values from 100 to 200) = 16 777 216 (total
			amount of numbers) / 101 (their possible values) - expected result in out_stats*//*
			if (fprintf(fp,  "%i\t%llu\t%i\n", i, out_stats[i], 166111) < 0) {
				error("cannot write to 'uint16_bruteforce.ods' file");
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
	//write a random numbers to original array
	if (!RAND_bytes((unsigned char *)orig_array, BYTESIZE))
    	OpenSSL_error();
    memset(in_stats, 0, sizeof(in_stats));			//initialize statistics arrays
    memset(out_stats, 0, sizeof(out_stats));
    stats_uint8_array(orig_array, size, in_stats);	//get a statistics on a pseudorandom numbers
      
	//let orig_array contain numbers from 100 to 219 distributed uniformly
	for (j = 0; j < size; j++) {
		//write a fresh random byte to this position until it will be between 0 and 239
		while (orig_array[j] > 239) {
			if (!RAND_bytes((unsigned char *)(orig_array+j), 1))
	    		OpenSSL_error();
			}
				
		orig_array[j] = (orig_array[j] % 120) + 100;
		}
	
	encode_uint16_uniform(orig_array, encoded_array, size, 100, 219);
	//get a statistics on an encoded array
	stats_uint8_array((uint8_t *)encoded_array, 2*BYTESIZE, out_stats);
	decode_uint16_uniform(encoded_array, decoded_array, size, 100, 219);
	if (memcmp(orig_array, decoded_array, size)) {
		error("orig_array and decoded_array are not the same");
		test_error();
		}
	
	//write statistics to file
	//try to open file for writing
	if ((fp = fopen("uint16_encoding.ods", "w")) == NULL) {	
		error("can't open file 'uint16_encoding.ods' for writing");
		test_error();
		}
	
	//compare pseudorandom vs. ideal, actual vs. ideal distributions
	if (fprintf(fp, "\t=CHITEST(B2:B257;C2:C257)\t\t=CHITEST(D2:D257;E2:E257)\n") < 0) {
		error("cannot write to 'uint16_encoding.ods' file");
		if (fclose(fp) == EOF)
			perror("test: fclose error");
		test_error();
		}
	//write four columns to file: pseudorandom and ideal, actual and ideal distributions for CHITEST
	for (i = 0; i <= UINT8_MAX; i++) {
		if (fprintf(fp, "%i\t%llu\t%i\t%llu\t%i\n", i, in_stats[i], size/256, out_stats[i], size/128) < 0) {
			error("cannot write to 'uint16_encoding.ods' file");
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
		if (!RAND_bytes((unsigned char *)orig_array, BYTESIZE))
    		OpenSSL_error();
    	
   		//let orig_array contain numbers from 100 to 199 distributed uniformly
		for (j = 0; j < size; j++) {
			//write a fresh random byte to this position until it will be between 0 and 199
			while (orig_array[j] > 199) {
				if (!RAND_bytes((unsigned char *)(orig_array+j), 1))
		    		OpenSSL_error();
				}
				
			orig_array[j] = (orig_array[j] % 100) + 100;
			}
		
		encode_uint16_uniform(orig_array, encoded_array, size, 100, 199);
		stats_uint16_array(encoded_array, size, long_stats);
		}
	
	//write overall statistics to file
	//try to open file for writing
	if ((fp = fopen("uint16_encoding2.ods", "w")) == NULL) {
		error("can't open file 'uint16_encoding2.ods' for writing");
	    test_error();
		}
		
	//compare actual vs. ideal distributions of output array
	if (fprintf(fp, "\t=CHITEST(B2:B65537;C2:C65537)\n") < 0) {
		error("cannot write to 'uint16_encoding2.ods' file");
		if (fclose(fp) == EOF)
			perror("test: fclose error");
		test_error();
		}
	//write two columns to file: actual and ideal distribution for CHITEST
	for (i = 0; i <= UINT16_MAX; i++) {*/
		/*256 = 65 536 (number of encodings) * 256 (size of each array for encoding) / 65536
		(number of possible array values from 0 to 65 535) = 16 777 216 (total amount of
		numbers) / 65536 (their possible values) - expected result in long_stats*//*
		if (fprintf(fp,  "%i\t%llu\t%i\n", i, long_stats[i], 256) < 0) {
			error("cannot write to 'uint16_encoding2.ods' file");
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
	*/
	
	
	//random encoding and decoding-----------------------------------------------------------------
	for (size = 1; size <= UINT8_MAX; size++) {
		//write a random numbers to original array
		if (!RAND_bytes((unsigned char *)orig_array, BYTESIZE))
    		OpenSSL_error();
    	get_uint16_minmax(orig_array, size, &min, &max);
		encode_uint16_uniform(orig_array, encoded_array, size, min, max);
    	decode_uint16_uniform(encoded_array, decoded_array, size, min, max);
    	if (memcmp(orig_array, decoded_array, size)) {
    		error("orig_array and decoded_array are not the same");
			print_uint16_array(orig_array, size);
			print_uint16_array(decoded_array, size);
			test_error();
			}
		}
	
	
	
	//fixed general cases--------------------------------------------------------------------------
	
	size = 5;
	orig_array[0] = 2000;
	orig_array[1] = 2500;
	orig_array[2] = 3000;
	orig_array[3] = 3500;
	orig_array[4] = 4000;
	
	printf("Original array:\n");	//print it
	print_uint16_array(orig_array, size);
	get_uint16_minmax(orig_array, size, &min, &max);
	
	printf("min = %i, max = %i:\n", min, max);
	encode_uint16_uniform(orig_array, encoded_array, size, min, max);
	print_uint32_array(encoded_array, size);
	decode_uint16_uniform(encoded_array, decoded_array, size, min, max);
	//if original and decoded arrays is not equal then print a decoded array too
	if (memcmp(orig_array, decoded_array, size))
		print_uint16_array(decoded_array, size);
	
	printf("min = 1500, max = 4500:\n");
	encode_uint16_uniform(orig_array, encoded_array, size, 1500, 4500);
	print_uint32_array(encoded_array, size);
	decode_uint16_uniform(encoded_array, decoded_array, size, 1500, 4500);
	if (memcmp(orig_array, decoded_array, size))
		print_uint16_array(decoded_array, size);
	
	printf("min = 0, max = %i:\n", UINT16_MAX);
	encode_uint16_uniform(orig_array, encoded_array, size, 0, UINT16_MAX);
	print_uint32_array(encoded_array, size);
	decode_uint16_uniform(encoded_array, decoded_array, size, 0, UINT16_MAX);
	if (memcmp(orig_array, decoded_array, size))
		print_uint16_array(decoded_array, size);
	printf("\n");
	
	
	
	//fixed special cases--------------------------------------------------------------------------
	
	size = 5;
	orig_array[0] = 2000;
	orig_array[1] = 2000;
	orig_array[2] = 2000;
	orig_array[3] = 2000;
	orig_array[4] = 2000;
	
	printf("Original array:\n");
	print_uint16_array(orig_array, size);
	get_uint16_minmax(orig_array, size, &min, &max);
	
	printf("min = %i, max = %i:\n", min, max);
	encode_uint16_uniform(orig_array, encoded_array, size, min, max);
	print_uint32_array(encoded_array, size);
	decode_uint16_uniform(encoded_array, decoded_array, size, min, max);
	if (memcmp(orig_array, decoded_array, size))
		print_uint16_array(decoded_array, size);
	
	printf("min = 0, max = %i:\n", UINT16_MAX);
	encode_uint16_uniform(orig_array, encoded_array, size, 0, UINT16_MAX);
	print_uint32_array(encoded_array, size);
	decode_uint16_uniform(encoded_array, decoded_array, size, 0, UINT16_MAX);
	if (memcmp(orig_array, decoded_array, size))
		print_uint16_array(decoded_array, size);
	printf("\n");
	
	
	
	//wrong parameters-----------------------------------------------------------------------------
	get_uint16_minmax(NULL, 0, NULL, NULL);
	get_uint16_minmax(orig_array, 0, NULL, NULL);
	get_uint16_minmax(orig_array, 1, NULL, NULL);
	get_uint16_minmax(orig_array, 1, &min, NULL);
	printf("\n");
	
	encode_uint16_uniform(NULL, NULL, 0, 0, 0);
	encode_uint16_uniform(orig_array, NULL, 0, 0, 0);
	encode_uint16_uniform(orig_array, encoded_array, 0, 0, 0);
	encode_uint16_uniform(orig_array, encoded_array, 1, 2, 1);
	encode_uint16_uniform(orig_array, encoded_array, size, 2100, 10000);
	encode_uint16_uniform(orig_array, encoded_array, size, 0, 1900);
	printf("\n");
	
	decode_uint16_uniform(NULL, NULL, 0, 0, 0);
	decode_uint16_uniform(encoded_array, NULL, 0, 0, 0);
	decode_uint16_uniform(encoded_array, orig_array, 0, 0, 0);
	decode_uint16_uniform(encoded_array, orig_array, 1, 2, 1);
	printf("\n");
	
	stats_uint16_array(NULL, 0, NULL);
	stats_uint16_array(orig_array, 0, NULL);
	stats_uint16_array(orig_array, 1, NULL);
	printf("\n");
	
	print_uint16_array(NULL, 0);
	print_uint16_array(orig_array, 0);
	printf("\n");
	
	print_uint32_array(NULL, 0);
	print_uint32_array(encoded_array, 0);
	
	
	
	#undef BYTESIZE
	test_deinit();
	
	return 0;
	
}
