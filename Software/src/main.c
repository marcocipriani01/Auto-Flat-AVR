#include "main.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s /dev/ttyUSBx", argv[0]);
        return 1;
    }
    
    char* serialPort = argv[1];
    printf("Opening serial port %s... ", serialPort);
    int fd = serialOpen(serialPort, 9600);
    if ((fd <= 0) && serialSetBlocking(fd, 1)) {
        printf("Error opening the serial port!\n");
        return 1;
    } else {
        printf("Connected.\n");
    }

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
