/*
test_common.h - common code for test programs for honeydata library
License: BSD 2-Clause
*/

#include "hdata/hd_int_uniform.h"

extern void error_handler(void);
//encrypt a message
extern int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
	unsigned char *iv, unsigned char *ciphertext);
//decrypt a message
extern int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
	unsigned char *iv, unsigned char *plaintext);
extern int8_t print_uint8_array(const uint8_t *array, const uint64_t size);
extern int8_t print_uint16_array(const uint16_t *array, const uint64_t size);
//get a number of occurences of different elements in array
extern int8_t stats_uint8_array(const uint8_t *in_array, const uint64_t size, uint64_t *stats);
extern int8_t stats_uint16_array(const uint16_t *in_array, const uint64_t size, uint64_t *stats);
