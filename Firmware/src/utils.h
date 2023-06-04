#ifndef UTILS_H
#define UTILS_H

#include <inttypes.h>

int32_t constrain(int32_t x, int32_t low, int32_t high);
int32_t linearMap(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max);

#endif
