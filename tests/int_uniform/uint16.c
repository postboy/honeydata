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
	#define ITYPE uint16_t							//type for testing in this test unit
	#define PRI PRIu16								//macro for printing it
	#define OTYPE uint32_t							//type of container in this test unit
	#define BYTESIZE (size*sizeof(ITYPE))			//current input array size in bytes
	const size_t maxsize = 1048576/sizeof(ITYPE);	//maximum array size (1MB)
	//statistics on pseudorandom and output arrays
	uint64_t in_stats[256] = {0}, out_stats[256] = {0}, long_stats[65536] = {0};
	ITYPE min, max, orig_array[maxsize], decoded_array[maxsize];	//minimum and maximim in array
	OTYPE encoded_array[maxsize];
	FILE *fp;
	
	/*Buffer for ciphertext. Ensure the buffer is long enough for the ciphertext which may be
	longer than the plaintext, dependant on the algorithm and mode (for AES-256 in CBC mode we need
	one extra block)*/
	unsigned char ciphertext[2*256*sizeof(ITYPE)];
	size_t decryptedtext_len, ciphertext_len;		//their lengths
	
	//variables for bruteforce test
	uint16_t bfkey;				//current iteration number
	unsigned char big_key[32];	//current key
	
	test_init();
	
	
	
	//random data encoding, encryption, decryption, decoding---------------------------------------
	
	size = 256;
	//write a random numbers to original array
	randombytes((unsigned char *)orig_array, BYTESIZE);
	
	//let orig_array contain numbers from 1000 to 1999
	for (i = 0; i < size; i++) {
		//write a fresh random element to this position until it will be between 0 and 64999
		while (orig_array[i] > 64999)
			randombytes((unsigned char *)(orig_array+i), sizeof(ITYPE));
		orig_array[i] = (orig_array[i] % 1000) + 1000;
		}
	
	get_uint16_minmax(orig_array, size, &min, &max);
	encode_uint16_uniform(orig_array, encoded_array, size, min, max);
	ciphertext_len = encrypt((unsigned char *)encoded_array, 2*BYTESIZE, key, iv, ciphertext);
	decryptedtext_len = decrypt(ciphertext, ciphertext_len, key, iv, (unsigned char *)encoded_array);
	decode_uint16_uniform(encoded_array, decoded_array, size, min, max);
	
	//compare result of decryption and original array
	if ( memcmp(orig_array, decoded_array, BYTESIZE) || (decryptedtext_len != 2*BYTESIZE) ) {
		error("orig_array and decoded_array are not the same");
		printf("size = %zu, decryptedtext_len = %zu\n", size, decryptedtext_len);
		print_uint16_array(orig_array, 10);
		print_uint16_array(decoded_array, 10);
		test_error();
		}
	
	
	
	//bruteforce test (complexity equals 2^16) with statistics collection--------------------------
	
	size = 256;
	
	//encode and encrypt data
	encode_uint16_uniform(orig_array, encoded_array, size, 1000, 1999);
	ciphertext_len = encrypt((unsigned char *)encoded_array, 2*BYTESIZE, key, iv, ciphertext);
	
	//let's try to bruteforce last 2 bytes of a key
	bfkey = 0;
	memcpy(big_key, key, 30);					//suppose that we know 30 first bytes of key
	memset(long_stats, 0, sizeof(long_stats));	//initialize statistics arrays
	
	for (i = 0; i < 65536; i++) {
		memcpy((void *)(big_key+30), &bfkey, sizeof(bfkey));	//get current key for decryption
		bfkey++;												//try next key on next iteration
		decryptedtext_len = decrypt(ciphertext, ciphertext_len, big_key, iv,
									(unsigned char *)encoded_array);
		decode_uint16_uniform(encoded_array, decoded_array, size, 1000, 1999);
		if (decryptedtext_len != 2*BYTESIZE) {
			error("wrong decryptedtext_len value");
			printf("size = %zu, decryptedtext_len = %zu\n", size, decryptedtext_len);
			test_error();
			}
		//get a statistics on current bruteforce iteration
		stats_uint16_array(decoded_array, size, long_stats);
		/*
		int32_t j;
		for (j = 0; j < 32; j++) {				//print current key
			printf("%02x", big_key[j]);			//output format is HEX-code
			if ( (j+1) % 4 == 0) printf(" ");	//place a space every 2 bytes
			}
		printf("\n");
		*/
		}
		
	//write overall bruteforce statistics to file
	//try to open file for writing
	if ((fp = fopen("uint16_bruteforce.ods", "w")) == NULL) {
		error("can't open file 'uint16_bruteforce.ods' for writing");
		test_error();
		}
		
	//compare actual vs. ideal distributions of output array
	if (fprintf(fp, "\t=CHITEST(B12:B1011;C12:C1011)\n") < 0) {
		error("cannot write to 'uint16_bruteforce.ods' file");
		if (fclose(fp) == EOF)
			perror("test: fclose error");
		test_error();
		}
	//write two columns to file: actual and ideal distribution for CHITEST
	for (i = 990; i <= 2010; i++) {
		if ( (i < 1000) || (i > 1999) ) {
			if (fprintf(fp, "%i\t%"PRIu64"\t%i\n", i, long_stats[i], 0) < 0) {
				error("cannot write to 'uint16_bruteforce.ods' file");
				if (fclose(fp) == EOF)
					perror("test: fclose error");
				test_error();
				}
			}
		else
			/*16 777 = 65 536 (number of keys in brutforce) * 256 (size of each decrypted text in
			elements) / 1000 (number of possible array values from 1000 to 1999) = 16 777 216 (total
			amount of numbers) / 1000 (their possible values) - expected result in long_stats*/
			if (fprintf(fp, "%i\t%"PRIu64"\t%i\n", i, long_stats[i], 16777) < 0) {
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
	randombytes((unsigned char *)orig_array, BYTESIZE);
	memset(in_stats, 0, sizeof(in_stats));			//initialize statistics arrays
	memset(out_stats, 0, sizeof(out_stats));
	//get a statistics on a pseudorandom numbers
	stats_uint8_array((uint8_t *)orig_array, BYTESIZE, in_stats);
	/*write a fresh random numbers to original array and get a statistics on them again for fair
	comparison with 2*BYTESIZE encoded bytes below*/
	randombytes((unsigned char *)orig_array, BYTESIZE);
	stats_uint8_array((uint8_t *)orig_array, BYTESIZE, in_stats);
	
	//let orig_array contain uniformly distributed numbers from 11000 to 20999
	for (i = 0; i < size; i++) {
		//write a fresh random element to this position until it will be between 0 and 59999
		while (orig_array[i] > 59999)
			randombytes((unsigned char *)(orig_array+i), sizeof(ITYPE));	
		orig_array[i] = (orig_array[i] % 10000) + 11000;
		}
	
	encode_uint16_uniform(orig_array, encoded_array, size, 11000, 20999);
	//get a statistics on an encoded array
	stats_uint8_array((uint8_t *)encoded_array, 2*BYTESIZE, out_stats);
	decode_uint16_uniform(encoded_array, decoded_array, size, 11000, 20999);
	if (memcmp(orig_array, decoded_array, BYTESIZE)) {
		error("orig_array and decoded_array are not the same");
		print_uint16_array(orig_array, 10);
		print_uint16_array(decoded_array, 10);
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
		if (fprintf(fp, "%i\t%"PRIu64"\t%i\t%"PRIu64"\t%i\n", i, in_stats[i], BYTESIZE/128,
					out_stats[i], BYTESIZE/128) < 0) {
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
	
	
	
	//random encoding and decoding-----------------------------------------------------------------
	
	for (size = 1; size < 256; size++) {
		//write a random numbers to original array
		randombytes((unsigned char *)orig_array, BYTESIZE);
		get_uint16_minmax(orig_array, size, &min, &max);
		encode_uint16_uniform(orig_array, encoded_array, size, min, max);
		decode_uint16_uniform(encoded_array, decoded_array, size, min, max);
		if (memcmp(orig_array, decoded_array, BYTESIZE)) {
			error("orig_array and decoded_array are not the same");
			print_uint16_array(orig_array, 10);
			print_uint16_array(decoded_array, 10);
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
	
	printf("min = %"PRI", max = %"PRI":\n", min, max);
	encode_uint16_uniform(orig_array, encoded_array, size, min, max);
	print_uint32_array(encoded_array, size);
	decode_uint16_uniform(encoded_array, decoded_array, size, min, max);
	//if original and decoded arrays are not equal then print a decoded array too
	if (memcmp(orig_array, decoded_array, BYTESIZE))
		print_uint16_array(decoded_array, size);
	
	printf("min = 1500, max = 4500:\n");
	encode_uint16_uniform(orig_array, encoded_array, size, 1500, 4500);
	print_uint32_array(encoded_array, size);
	decode_uint16_uniform(encoded_array, decoded_array, size, 1500, 4500);
	if (memcmp(orig_array, decoded_array, BYTESIZE))
		print_uint16_array(decoded_array, size);
	
	printf("min = 0, max = %"PRI":\n", UINT16_MAX);
	encode_uint16_uniform(orig_array, encoded_array, size, 0, UINT16_MAX);
	print_uint32_array(encoded_array, size);
	decode_uint16_uniform(encoded_array, decoded_array, size, 0, UINT16_MAX);
	if (memcmp(orig_array, decoded_array, BYTESIZE))
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
	
	printf("min = %"PRI", max = %"PRI":\n", min, max);
	encode_uint16_uniform(orig_array, encoded_array, size, min, max);
	print_uint32_array(encoded_array, size);
	decode_uint16_uniform(encoded_array, decoded_array, size, min, max);
	if (memcmp(orig_array, decoded_array, BYTESIZE))
		print_uint16_array(decoded_array, size);
	
	printf("min = 0, max = %"PRI":\n", UINT16_MAX);
	encode_uint16_uniform(orig_array, encoded_array, size, 0, UINT16_MAX);
	print_uint32_array(encoded_array, size);
	decode_uint16_uniform(encoded_array, decoded_array, size, 0, UINT16_MAX);
	if (memcmp(orig_array, decoded_array, BYTESIZE))
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
	
	
	
	#undef ITYPE
	#undef PRI
	#undef OTYPE
	#undef BYTESIZE
	test_deinit();
	
	return 0;
}
