#include <stdio.h>
#include <stdlib.h>

#include "serial.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("usage: %s <filename> <baudrate>", argv[0]);
        return 0;
    }
    char* filename = argv[1];
    printf("opening serial device [%s] ... ", filename);
    int fd = serial_open(filename);
    if (fd <= 0) {
        printf("Error\n");
        return 0;
    } else {
        printf("Success\n");
    }
    int attribs = serial_set_interface_attribs(fd, 9600, 0);
    if (attribs) {
        printf("Error\n");
        return 0;
    }

    serial_set_blocking(fd, 1);

    const int bsize = 10;
    char buf[bsize];
    while (1) {
        int n = write(fd, ">O\r", 3);
        if (n < 0)
            printf("write() failed!\n");

        int n_read = read(fd, buf, bsize);
        for (int i = 0; i < n_read; ++i) {
            printf("%c", buf[i]);
        }
    }
}
