#pragma once
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

//! returns the descriptor of a serial port
int serial_open(const char* name);

//! sets the attributes
int serial_set_interface_attribs(int fd, int speed, int parity);

//! puts the port in blocking/nonblocking mode
void serial_set_blocking(int fd, int should_block);
