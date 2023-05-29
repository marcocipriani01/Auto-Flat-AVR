#ifndef UTILS_H
#define UTILS_H

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define constrain(x, low, high) ((x) < (low) ? (low) : ((x) > (high) ? (high) : (x)))

int linearMap(int x, int in_min, int in_max, int out_min, int out_max);

#endif
