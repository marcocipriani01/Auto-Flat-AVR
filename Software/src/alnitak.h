#pragma once

#include <string.h>

// Alnitak device IDs
#define FLAT_MAN_L 10
#define FLAT_MAN_XL 15
#define FLAT_MAN 19
#define FLIP_FLAT 99

typedef enum {
    MOVING = 0,
    CLOSED = 1,
    OPEN = 2
} ShutterStatus;

void shutterStatusString(ShutterStatus status, char *buf);
void deviceIDString(int deviceId, char *buf);
