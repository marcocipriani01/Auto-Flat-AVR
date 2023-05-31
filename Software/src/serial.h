#pragma once

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <termios.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

/**
 * Opens a serial port and returns its file descriptor.
 */
int serialOpen(const char* port, int baudrate);

/**
 * Enables or disables serial port blocking mode.
 */
bool serialSetBlocking(int fd, bool block);

/**
 * Closes a serial port.
 */
void serialClose(int fd);

/**
 * Writes a string to a serial port.
 */
void serialPrint(int fd, const char* str);
