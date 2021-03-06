/*
components for uniformly distributed floating-point numbers
license: BSD 2-Clause
*/

#ifndef HD_INT_UNIFORM_H
#define HD_INT_UNIFORM_H

#include "hd_common.h"
#include "hd_int_arbitrary.h"

//functions for float type arrays
/*convert fp number to integer and back such way that if (fp1 < fp2) then (int1 < int2), if
(fp1 = fp2) then (int1 = int2), if (fp1 > fp2) then (int1 > int2), i.e. each integer value will
contain a number of corresponding fp number in sequence of all possible fp numbers of this type*/
extern int float_to_uint32_uniform(const float *in_array, uint32_t *out_array, const size_t size);
extern int uint32_to_float_uniform(const uint32_t *in_array, float *out_array, const size_t size);

//for double type arrays
extern int double_to_uint64_uniform(const double *in_array, uint64_t *out_array, const size_t size);
extern int uint64_to_double_uniform(const uint64_t *in_array, double *out_array, const size_t size);

//check what container can be used for such array
extern int container_float_uniform(const float min, const float max);

#endif
