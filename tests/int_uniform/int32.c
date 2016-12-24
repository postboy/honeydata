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
	#define ITYPE int32_t							//type for testing in this test unit
	#define PRI PRIi32								//macro for printing it
	#define OTYPE uint64_t							//type of container in this test unit
	#define BYTESIZE (size*sizeof(ITYPE))			//current input array size in bytes
	const size_t maxsize = 1048576/sizeof(ITYPE);	//maximum array size (1MB)
	//statistics on pseudorandom and output arrays
	uint64_t in_stats[256] = {0}, out_stats[256] = {0};
	ITYPE min, max, orig_array[maxsize], decoded_array[maxsize];	//minimum and maximim in array
	OTYPE encoded_array[maxsize];
	FILE *fp;
	
	/*Buffer for ciphertext. Ensure the buffer is long enough for the ciphertext which may be
	longer than the plaintext, dependant on the algorithm and mode (for AES-256 in CBC mode we need
	one extra block)*/
	unsigned char ciphertext[2*256*sizeof(ITYPE)];
	size_t decryptedtext_len, ciphertext_len;		//their lengths
	
	test_init();
	
	
	
	//random data encoding, encryption, decryption, decoding---------------------------------------
	
	size = 256;
	//write a random numbers to original array
	randombytes((unsigned char *)orig_array, BYTESIZE);
	
	//let orig_array contain numbers from -1000000000 to 1000000000
	for (i = 0; i < size; i++) {
		/*write a fresh random element to this position until it will be between -1000000000 and
		1000000000*/
		while ( (orig_array[i] < -1000000000) || (orig_array[i] > 1000000000) )
			randombytes((unsigned char *)(orig_array+i), sizeof(ITYPE));
		}
	
	get_int32_minmax(orig_array, size, &min, &max);
	encode_int32_uniform(orig_array, encoded_array, size, min, max);
	ciphertext_len = encrypt((unsigned char *)encoded_array, 2*BYTESIZE, key, iv, ciphertext);
	decryptedtext_len = decrypt(ciphertext, ciphertext_len, key, iv, (unsigned char *)encoded_array);
	decode_int32_uniform(encoded_array, decoded_array, size, min, max);
	
	//compare result of decryption and original array
	if ( memcmp(orig_array, decoded_array, BYTESIZE) || (decryptedtext_len != 2*BYTESIZE) ) {
		error("orig_array and decoded_array are not the same");
		printf("size = %zu, decryptedtext_len = %zu\n", size, decryptedtext_len);
		print_int32_array(orig_array, 10);
		print_int32_array(decoded_array, 10);
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
	comparsion with 2*BYTESIZE encoded bytes below*/
	randombytes((unsigned char *)orig_array, BYTESIZE);
	stats_uint8_array((uint8_t *)orig_array, BYTESIZE, in_stats);
	
	//let orig_array contain numbers from -2000000000 to 1000000000 distributed uniformly
	for (i = 0; i < size; i++) {
		/*write a fresh random element to this position until it will be between -2000000000 and
		1000000000*/
		while ( (orig_array[i] < -2000000000) || (orig_array[i] > 1000000000) )
			randombytes((unsigned char *)(orig_array+i), sizeof(ITYPE));
		}
	
	encode_int32_uniform(orig_array, encoded_array, size, -2000000000, 1000000000);
	//get a statistics on an encoded array
	stats_uint8_array((uint8_t *)encoded_array, 2*BYTESIZE, out_stats);
	decode_int32_uniform(encoded_array, decoded_array, size, -2000000000, 1000000000);
	if (memcmp(orig_array, decoded_array, BYTESIZE)) {
		error("orig_array and decoded_array are not the same");
		print_int32_array(orig_array, 10);
		print_int32_array(decoded_array, 10);
		test_error();
		}
	
	//write statistics to file
	//try to open file for writing
	if ((fp = fopen("int32_encoding.ods", "w")) == NULL) {	
		error("can't open file 'int32_encoding.ods' for writing");
		test_error();
		}
	
	//compare pseudorandom vs. ideal, actual vs. ideal distributions
	if (fprintf(fp, "\t=CHITEST(B2:B257;C2:C257)\t\t=CHITEST(D2:D257;E2:E257)\n") < 0) {
		error("cannot write to 'int32_encoding.ods' file");
		if (fclose(fp) == EOF)
			perror("test: fclose error");
		test_error();
		}
	//write four columns to file: pseudorandom and ideal, actual and ideal distributions for CHITEST
	for (i = 0; i <= UINT8_MAX; i++) {
		if (fprintf(fp, "%i\t%"PRIu64"\t%i\t%"PRIu64"\t%i\n", i, in_stats[i], BYTESIZE/128,
					out_stats[i], BYTESIZE/128) < 0) {
			error("cannot write to 'int32_encoding.ods' file");
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
		get_int32_minmax(orig_array, size, &min, &max);
		encode_int32_uniform(orig_array, encoded_array, size, min, max);
		decode_int32_uniform(encoded_array, decoded_array, size, min, max);
		if (memcmp(orig_array, decoded_array, BYTESIZE)) {
			error("orig_array and decoded_array are not the same");
			print_int32_array(orig_array, 10);
			print_int32_array(decoded_array, 10);
			test_error();
			}
		}
	
	
	
	//fixed general cases--------------------------------------------------------------------------
	
	size = 5;
	orig_array[0] = -2000000000;
	orig_array[1] = -1900000000;
	orig_array[2] = -1700000000;
	orig_array[3] = -1200000000;
	orig_array[4] = -1000000000;
	
	printf("Original array:\n");	//print it
	print_int32_array(orig_array, size);
	get_int32_minmax(orig_array, size, &min, &max);
	
	printf("min = %"PRI", max = %"PRI":\n", min, max);
	encode_int32_uniform(orig_array, encoded_array, size, min, max);
	print_uint64_array(encoded_array, size);
	decode_int32_uniform(encoded_array, decoded_array, size, min, max);
	//if original and decoded arrays are not equal then print a decoded array too
	if (memcmp(orig_array, decoded_array, BYTESIZE))
		print_int32_array(decoded_array, size);
	
	printf("min = -2100000000, max = -800000000:\n");
	encode_int32_uniform(orig_array, encoded_array, size, -2100000000, -800000000);
	print_uint64_array(encoded_array, size);
	decode_int32_uniform(encoded_array, decoded_array, size, -2100000000, -800000000);
	if (memcmp(orig_array, decoded_array, BYTESIZE))
		print_int32_array(decoded_array, size);
	
	printf("min = %"PRI", max = %"PRI":\n", INT32_MIN, INT32_MAX);
	encode_int32_uniform(orig_array, encoded_array, size, INT32_MIN, INT32_MAX);
	print_uint64_array(encoded_array, size);
	decode_int32_uniform(encoded_array, decoded_array, size, INT32_MIN, INT32_MAX);
	if (memcmp(orig_array, decoded_array, BYTESIZE))
		print_int32_array(decoded_array, size);
	printf("\n");
	
	
	
	//fixed special cases--------------------------------------------------------------------------
	
	size = 5;
	orig_array[0] = -2000000000;
	orig_array[1] = -2000000000;
	orig_array[2] = -2000000000;
	orig_array[3] = -2000000000;
	orig_array[4] = -2000000000;
	
	printf("Original array:\n");
	print_int32_array(orig_array, size);
	get_int32_minmax(orig_array, size, &min, &max);
	
	printf("min = %"PRI", max = %"PRI":\n", min, max);
	encode_int32_uniform(orig_array, encoded_array, size, min, max);
	print_uint64_array(encoded_array, size);
	decode_int32_uniform(encoded_array, decoded_array, size, min, max);
	if (memcmp(orig_array, decoded_array, BYTESIZE))
		print_int32_array(decoded_array, size);
	
	printf("min = %"PRI", max = %"PRI":\n", INT32_MIN, INT32_MAX);
	encode_int32_uniform(orig_array, encoded_array, size, INT32_MIN, INT32_MAX);
	print_uint64_array(encoded_array, size);
	decode_int32_uniform(encoded_array, decoded_array, size, INT32_MIN, INT32_MAX);
	if (memcmp(orig_array, decoded_array, BYTESIZE))
		print_int32_array(decoded_array, size);
	printf("\n");
	
	
	
	//wrong parameters-----------------------------------------------------------------------------
	
	get_int32_minmax(NULL, 0, NULL, NULL);
	get_int32_minmax(orig_array, 0, NULL, NULL);
	get_int32_minmax(orig_array, 1, NULL, NULL);
	get_int32_minmax(orig_array, 1, &min, NULL);
	printf("\n");
	
	encode_int32_uniform(NULL, NULL, 0, 0, 0);
	encode_int32_uniform(orig_array, NULL, 0, 0, 0);
	encode_int32_uniform(orig_array, encoded_array, 0, 0, 0);
	encode_int32_uniform(orig_array, encoded_array, 1, 2, 1);
	encode_int32_uniform(orig_array, encoded_array, size, -1900000000, 0);
	encode_int32_uniform(orig_array, encoded_array, size, -2140000000, -2100000000);
	printf("\n");
	
	decode_int32_uniform(NULL, NULL, 0, 0, 0);
	decode_int32_uniform(encoded_array, NULL, 0, 0, 0);
	decode_int32_uniform(encoded_array, orig_array, 0, 0, 0);
	decode_int32_uniform(encoded_array, orig_array, 1, 2, 1);
	printf("\n");
	
	print_int32_array(NULL, 0);
	print_int32_array(orig_array, 0);
	
	
	
	#undef ITYPE
	#undef PRI
	#undef OTYPE
	#undef BYTESIZE
	test_deinit();
	
	return 0;
}
