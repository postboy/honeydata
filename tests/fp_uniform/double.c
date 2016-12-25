/*
test program for honeydata library
license: BSD 2-Clause
*/

#include "../t_common.h"
#include "../../hdata/hd_fp_uniform.h"

extern int main(void)
{
	
	int32_t i;
	size_t size;										//current array size
	#define TYPE double									//type for testing in this test unit
	#define PRI "%g"									//macro for printing it
	#define BYTESIZE (size*sizeof(TYPE))				//current input array size in bytes
	const size_t maxsize = 5000;						//maximum array size
	TYPE orig_array[maxsize], decoded_array[maxsize];	//minimum and maximim in array
	uint64_t encoded_array[maxsize];
	//uint8_t bad[] = {1, 0, 0, 0, 0, 0, 240, 255};		//signaling NaN
	
	test_init();
	
	
	
	//random encoding and decoding-----------------------------------------------------------------
	
	for (size = 1; size < maxsize; size++) {
		//write a random numbers to original array
		randombytes((unsigned char *)orig_array, BYTESIZE);
		/*memcpy(orig_array, bad, sizeof(TYPE));
		print_uint8_array((uint8_t *)orig_array, sizeof(TYPE));*/
		//convert signaling NaNs to quiet NaNs to avoid errors with comparsion
		to_quiet_nans_double(orig_array, orig_array, size);
		//print_uint8_array((uint8_t *)orig_array, sizeof(TYPE));
		double_to_uint64_uniform(orig_array, encoded_array, size);
		uint64_to_double_uniform(encoded_array, decoded_array, size);
		if (memcmp(orig_array, decoded_array, BYTESIZE)) {
			error("orig_array and decoded_array are not the same");
			print_double_array(orig_array, size);
			print_double_array(decoded_array, size);
			print_uint8_array((uint8_t *)orig_array, BYTESIZE);
			print_uint8_array((uint8_t *)decoded_array, BYTESIZE);
			test_error();
			}
		}
	
	
	
	//comparison test------------------------------------------------------------------------------
	
	/*we tried to encode data such way that if (fl1 < fl2) then (int1 < int2), if (fl1 = fl2) then
	(int1 = int2), if (fl1 > fl2) then (int1 > int2), i.e. each integer value will contain a number
	of corresponding fp number in sequence of all possible fp numbers. let's check do we succeeded
	*/
	
	size = 10;
		
	for (i = 0; i < size-1; i++)
		if (encoded_array[i] > encoded_array[i+1])
			printf(PRI" > "PRI"\n", orig_array[i], orig_array[i+1]);
		else if (encoded_array[i] < encoded_array[i+1])
			printf(PRI" < "PRI"\n", orig_array[i], orig_array[i+1]);
		else
			printf(PRI" = "PRI"\n", orig_array[i], orig_array[i+1]);
	
	printf("\n");
	
	
	
	//wrong parameters-----------------------------------------------------------------------------
	
	double_to_uint64_uniform(NULL, NULL, 0);
	double_to_uint64_uniform(orig_array, NULL, 0);
	double_to_uint64_uniform(orig_array, encoded_array, 0);
	printf("\n");
	
	uint64_to_double_uniform(NULL, NULL, 0);
	uint64_to_double_uniform(encoded_array, NULL, 0);
	uint64_to_double_uniform(encoded_array, orig_array, 0);
	printf("\n");
	
	to_quiet_nans_double(NULL, NULL, 0);
	to_quiet_nans_double(orig_array, NULL, 0);
	to_quiet_nans_double(orig_array, decoded_array, 0);
	printf("\n");
	
	print_double_array(NULL, 0);
	print_double_array(orig_array, 0);
	
	
	
	#undef TYPE
	#undef PRI
	#undef BYTESIZE
	test_deinit();
	
	return 0;
	
}
