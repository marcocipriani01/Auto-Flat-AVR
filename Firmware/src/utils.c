#include "utils.h"

int abs(int x) {
    return (x < 0) ? (-x) : x;
}

int constrain(int x, int low, int high) {
    return (x < low) ? low : ((x > high) ? high : x);
}

int linearMap(int x, int in_min, int in_max, int out_min, int out_max) {
    int out = ((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
    if (out_min < out_max)
        return constrain(out, out_min, out_max);
    else
        return constrain(out, out_max, out_min);
}
