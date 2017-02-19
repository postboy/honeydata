/*
test program for honeydata library
license: BSD 2-Clause
*/

#include "../t_common.h"
#include "../../hdata/hd_int_arbitrary.h"

extern int main(void)
{
	#define ITYPE int64_t							//type for testing in this test unit
	#define BYTESIZE (size*sizeof(ITYPE))			//current input array size in bytes

	const ITYPE orig_array[] = {-9223372036854775710, -9223372036854775710, -9223372036854775710,
		-9223372036854775712, -9223372036854775712, -9223372036854775710, -9223372036854775710,
		-9223372036854775710, -9223372036854775712, -9223372036854775712};
	const size_t size = 10;
	ITYPE decoded_array[size], min, max;
	void *encoded_array;
	
	uint32_t weights[] = {2, 0, 3};
	int rv;
	
	//fixed general cases--------------------------------------------------------------------------

	get_int64_minmax(orig_array, size, &min, &max);
	
	if ((rv = encode_int64_arbitrary(orig_array, &encoded_array, size, min, max, weights)) != 2) {
		error("unexpected output type");
		printf("%d\n", rv);
		test_error();
		}
	decode_int64_arbitrary(encoded_array, decoded_array, size, min, max, weights);
	
	free(encoded_array);
	
	if (memcmp(orig_array, decoded_array, BYTESIZE)) {
		error("orig_array and decoded_array are not the same");
		print_int64_array(orig_array, size);
		print_int64_array(decoded_array, size);
		test_error();
		}
	
	
	weights[0] = 256;
	
	if ((rv = encode_int64_arbitrary(orig_array, &encoded_array, size, min, max, weights)) != 4) {
		error("unexpected output type");
		printf("%d\n", rv);
		test_error();
		}
	decode_int64_arbitrary(encoded_array, decoded_array, size, min, max, weights);
	
	free(encoded_array);
	
	if (memcmp(orig_array, decoded_array, BYTESIZE)) {
		error("orig_array and decoded_array are not the same");
		print_int64_array(orig_array, size);
		print_int64_array(decoded_array, size);
		test_error();
		}
	
	
	weights[0] = 65536;
	
	if ((rv = encode_int64_arbitrary(orig_array, &encoded_array, size, min, max, weights)) != 8) {
		error("unexpected output type");
		printf("%d\n", rv);
		test_error();
		}
	decode_int64_arbitrary(encoded_array, decoded_array, size, min, max, weights);
	
	free(encoded_array);
	
	if (memcmp(orig_array, decoded_array, BYTESIZE)) {
		error("orig_array and decoded_array are not the same");
		print_int64_array(orig_array, size);
		print_int64_array(decoded_array, size);
		test_error();
		}
	
	weights[0] = 4294967295;
	
	if ((rv = encode_int64_arbitrary(orig_array, &encoded_array, size, min, max, weights)) != -1) {
		error("unexpected output type");
		printf("%d\n", rv);
		test_error();
		}
	
	#undef ITYPE
	#undef BYTESIZE
	
	return 0;
}
