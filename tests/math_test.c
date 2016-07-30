/*
math_test.c - tests for some neat optimization trick in hd_int_uniform.c
License: BSD 2-Clause
*/

#include <stdio.h>
#include <inttypes.h>
#include <math.h>

#define FASTGROUPNUM(a, b, res) \
	do { \
		res = a / b + 1; \
		}\
	while (0)

extern int main(void)
{
	uint64_t i;
	
	for (i = 1; i <= (uint16_t)UINT8_MAX+1; i++) {
		uint16_t v1, v2;
		v1 = ceill( ((long double)UINT8_MAX + 1) / i);
		FASTGROUPNUM(UINT8_MAX, i, v2);
		if (v1 != v2) {
			printf("%"PRIu64": %"PRIu16" %"PRIu16"\n", i, v1, v2);
			goto end;
			}
		}
	printf("Tests for 8-bit integers successfully passed\n");

	for (i = 1; i <= (uint32_t)UINT16_MAX+1; i++) {
		uint32_t v1, v2;
		v1 = ceill( ((long double)UINT16_MAX + 1) / i);
		FASTGROUPNUM(UINT16_MAX, i, v2);
		if (v1 != v2) {
			printf("%"PRIu64": %"PRIu32" %"PRIu32"\n", i, v1, v2);
			goto end;
			}
		}
	printf("Tests for 16-bit integers successfully passed\n");
	
	//warning: slow!
	for (i = 1; i <= (uint64_t)UINT32_MAX+1; i++) {
		uint64_t v1, v2;
		v1 = ceill( ((long double)UINT32_MAX + 1) / i);
		FASTGROUPNUM(UINT32_MAX, i, v2);
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
