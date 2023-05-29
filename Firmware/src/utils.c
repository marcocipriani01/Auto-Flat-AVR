#include "utils.h"

int linearMap(int x, int in_min, int in_max, int out_min, int out_max) {
    int out = ((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
    if (out < out_min)
        return out_min;
    else if (out > out_max)
        return out_max;
}
