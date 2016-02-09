/*
test_common.h - common code for all test programs for honeydata library
License: BSD 2-Clause
*/

#include "lib/hd_int_uniform.h"

extern void error_handler(void);
//encrypt a message
extern int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv,
	unsigned char *ciphertext);
//decrypt a message
extern int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv,
	unsigned char *plaintext);
