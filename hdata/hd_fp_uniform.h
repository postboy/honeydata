/*
components for floating-point numbers distributed uniformly
license: BSD 2-Clause
*/

#ifndef HD_INT_UNIFORM_H
#define HD_INT_UNIFORM_H

#include "hd_common.h"

/*convert float to uint32 such way that if (fl1 < fl2) then (int1 < int2), if (fl1 = fl2) then
(int1 = int2), if (fl1 > fl2) then (int1 > int2), i.e. each uint32 value will contain a number of
corresponding float in sequence of all possible floats*/
extern int8_t float_to_uint32(const float *in_array, uint32_t *out_array, const size_t size);
extern int8_t uint32_to_float(const uint32_t *in_array, float *out_array, const size_t size);

#endif
