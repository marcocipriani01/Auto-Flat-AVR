#ifndef ALNITAK_H
#define ALNITAK_H

#include "config.h"

// Alnitak device IDs
#define FLAT_MAN_L 10
#define FLAT_MAN_XL 15
#define FLAT_MAN 19
#define FLIP_FLAT 99

#if SERVO_ENABLE == true
#define DEVICE_ID FLIP_FLAT

typedef enum {
    NEITHER_OPEN_NOR_CLOSED = 0,
    CLOSED,
    OPEN,
    TIMED_OUT
} ShutterStatus;

typedef enum {
    OPENING = 0,
    CLOSING,
    NONE
} MotorDirection;

typedef enum {
    STOPPED = 0,
    RUNNING
} MotorStatus;

#else
#define DEVICE_ID FLAT_MAN
#endif

#endif
