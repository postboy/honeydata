/*
math_test.c - tests for some neat optimization trick in hd_int_uniform.c
License: BSD 2-Clause
*/

#include <stdio.h>
#include <inttypes.h>
#include <math.h>

/*this test computationally proofs a following equation:
group_num = ceill( (long double)(OTYPE_MAX+1) / group_size) = OTYPE_MAX / group_size + 1.
in this test, OTYPE_MAX is UINT16_MAX, UINT32_MAX, and UINT64_MAX, respectively.
group_size is a value from 2 to (UINT8_MAX+1), (UINT16_MAX+1), and (UINT32_MAX+1), respectively.
group_size can also be 1 in library, but in this case we don't use group_num variable at all, so we
don't care about integer overflow there.
when we use floating point numbers, we use maximum accuracy available in C: conversion to long
double and ceill() function exactly for that type.*/

#define FASTGROUPNUM(a, b, res) \
	do { \
		res = a / b + 1; \
		}\
	while (0)

extern int main(void)
{
	uint64_t i;
	
	for (i = 2; i <= (uint16_t)UINT8_MAX+1; i++) {
		uint16_t v1, v2;
		v1 = ceill( ((long double)UINT16_MAX + 1) / i);
		FASTGROUPNUM(UINT16_MAX, i, v2);
		if (v1 != v2) {
			printf("%"PRIu64": %"PRIu16" %"PRIu16"\n", i, v1, v2);
			goto end;
			}
		}
	printf("Tests for 8-bit integers successfully passed\n");

	for (i = 2; i <= (uint32_t)UINT16_MAX+1; i++) {
		uint32_t v1, v2;
		v1 = ceill( ((long double)UINT32_MAX + 1) / i);
		FASTGROUPNUM(UINT32_MAX, i, v2);
		if (v1 != v2) {
			printf("%"PRIu64": %"PRIu32" %"PRIu32"\n", i, v1, v2);
			goto end;
			}
		}
	printf("Tests for 16-bit integers successfully passed\n");
	
	//warning: slow!
	for (i = 2; i <= (uint64_t)UINT32_MAX+1; i++) {
		uint64_t v1, v2;
		v1 = ceill( ((long double)UINT64_MAX + 1) / i);
		FASTGROUPNUM(UINT64_MAX, i, v2);
		if (v1 != v2) {
			printf("%"PRIu64": %"PRIu64" %"PRIu64"\n", i, v1, v2);
			goto end;
			}
		//show progress
		/*else
			if (i % 1000000 == 0)
				printf("%"PRIu64"\n", i);*/
		}
	printf("Tests for 32-bit integers successfully passed\n");

	end:
	return 0;
}
