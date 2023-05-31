#pragma once

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <termios.h>

/**
 * Opens a serial port and returns its file descriptor.
 */
int serialOpen(const char* port, int baudrate);

/**
 * Enables or disables serial port blocking mode.
 */
bool serialSetBlocking(int fd, bool block);
