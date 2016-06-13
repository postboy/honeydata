/*
test_common.h - common code for test programs for honeydata library
License: BSD 2-Clause
*/

#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include "../hdata/hd_int_uniform.h"

extern void error_handler(void);
//encrypt and decrypt a message
extern int encrypt(const unsigned char *plaintext, const size_t plaintext_len,
	const unsigned char *key, const unsigned char *iv, unsigned char *ciphertext);
extern int decrypt(const unsigned char *ciphertext, const size_t ciphertext_len,
	const unsigned char *key, const unsigned char *iv, unsigned char *plaintext);

//get a number of occurences of different elements in array
//functions for unsigned and signed 8 bit integers
extern int8_t stats_uint8_array(const uint8_t *in_array, const size_t size, uint64_t *stats);
extern int8_t stats_int8_array(const int8_t *in_array, const size_t size, uint64_t *stats);
//for unsigned 16 bit integers
extern int8_t stats_uint16_array(const uint16_t *in_array, const size_t size, uint64_t *stats);

//print a numeric array
//functions for unsigned and signed 8 bit integers
extern int8_t print_uint8_array(const uint8_t *array, const size_t size);
extern int8_t print_int8_array(const int8_t *array, const size_t size);
//for unsigned 16 bit integers
extern int8_t print_uint16_array(const uint16_t *array, const size_t size);

#endif
