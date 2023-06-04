#include "utils.h"

int32_t constrain(int32_t x, int32_t low, int32_t high) {
    if (x < low)
        return low;
    else if (x > high)
        return high;
    else
        return x;
}

int32_t linearMap(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max) {
    int32_t out = ((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
    if (out < out_min)
        return out_min;
    else if (out > out_max)
        return out_max;
    else
        return out;
}
