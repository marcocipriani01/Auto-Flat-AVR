#include "alnitak.h"

void shutterStatusString(ShutterStatus status, char *buf) {
    switch (status) {
        case MOVING:
            strncpy(buf, "moving", 7);
            break;
        case CLOSED:
            strncpy(buf, "closed", 7);
            break;
        case OPEN:
            strncpy(buf, "open", 5);
            break;
        default:
            strncpy(buf, "unknown", 8);
            break;
    }
}

void deviceIDString(int deviceId, char *buf) {
    switch (deviceId) {
        case FLAT_MAN_L:
            strncpy(buf, "Flat Man L", 11);
            break;
        case FLAT_MAN_XL:
            strncpy(buf, "Flat Man XL", 12);
            break;
        case FLAT_MAN:
            strncpy(buf, "Flat Man", 9);
            break;
        case FLIP_FLAT:
            strncpy(buf, "Flip-flat", 10);
            break;
        default:
            break;
    }
}
