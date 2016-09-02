/*
components for floating-point numbers distributed uniformly
license: BSD 2-Clause
*/

#ifndef HD_INT_UNIFORM_H
#define HD_INT_UNIFORM_H

#include "hd_common.h"

//functions for float type arrays
/*convert fp number to integer and back such way that if (fl1 < fl2) then (int1 < int2), if
(fl1 = fl2) then (int1 = int2), if (fl1 > fl2) then (int1 > int2), i.e. each integer value will
contain a number of corresponding fp number in sequence of all possible fp numbers*/
extern int8_t float_to_uint32(const float *in_array, uint32_t *out_array, const size_t size);
extern int8_t uint32_to_float(const uint32_t *in_array, float *out_array, const size_t size);

//for double type arrays
extern int8_t double_to_uint64(const double *in_array, uint64_t *out_array, const size_t size);
extern int8_t uint64_to_double(const uint64_t *in_array, double *out_array, const size_t size);

#endif
