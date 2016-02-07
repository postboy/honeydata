/*
test_common.h - common code for all test programs for honeydata library
License: BSD 2-Clause
*/

#include "lib/hd_int_uniform.h"

extern int8_t print_uint8_array(const uint8_t *array, const uint64_t size);
extern void error_handler(void);
//encrypt a message
extern int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv,
	unsigned char *ciphertext);
//decrypt a message
extern int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv,
	unsigned char *plaintext);
//get a number of occurences of different bytes in array
extern int8_t array_statistics(const unsigned char *in_array, const uint64_t size, uint64_t *stats);
