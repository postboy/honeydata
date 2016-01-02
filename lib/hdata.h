/*
hdata.h - honeydata library header
License: BSD 2-Clause
*/

#include <inttypes.h>

//DTE and DTD for unsigned 8 bit integers distributed uniformly
char encode_uint8_uniform(const unsigned char *in_array, unsigned char *out_array, uint64_t count);
char decode_uint8_uniform(const unsigned char *in_array, unsigned char *out_array, uint64_t count);
