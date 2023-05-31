#include "serial.h"

int serialOpen(const char* port, int baudrate) {
    int fd = open(port, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0)
        return 0;

    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0)
        return -1;
    switch (baudrate) {
        case 9600:
            baudrate = B9600;
            break;
        case 19200:
            baudrate = B19200;
            break;
        case 57600:
            baudrate = B57600;
            break;
        case 115200:
            baudrate = B115200;
            break;
        case 2500000:
            baudrate = B2500000;
            break;
        default:
            return 0;
    }
    cfsetospeed(&tty, baudrate);
    cfsetispeed(&tty, baudrate);
    cfmakeraw(&tty);
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag |= 0;
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;         // 8bit chars
    if (tcsetattr(fd, TCSANOW, &tty) != 0)
        return -2;

    return fd;
}

bool serialSetBlocking(int fd, bool block) {
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0)
        return false;
    
    tty.c_cc[VMIN] = block ? 1 : 0;
    tty.c_cc[VTIME] = 5;                                // 500ms read timeout
    if (tcsetattr(fd, TCSANOW, &tty) != 0)
        return false;

    return true;
}
