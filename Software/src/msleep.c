#include "msleep.h"

void msleep(unsigned long ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    int ret;
    do {
        ret = nanosleep(&ts, &ts);
    } while ((ret != 0) && (errno == EINTR));
}
