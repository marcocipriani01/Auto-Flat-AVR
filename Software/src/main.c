#include "main.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s /dev/ttyUSBx", argv[0]);
        return 1;
    }

    char* serialPort = argv[1];
    printf("Opening serial port %s... ", serialPort);
    int fd = serialOpen(serialPort, 9600);
    if ((fd > 0) && serialSetBlocking(fd, 1)) {
        printf("connected.\n");
    } else {
        printf("error opening the serial port!\n");
        return 1;
    }
    msleep(100);

    printf("Device info:\n");
    volatile bool gotFwVersion = false, gotSettings = false;
    int attempts = 0;
    const int buffSize = 20;
    char buff[buffSize];
    do {
        if (!gotFwVersion) {
            serialPrint(fd, ">V\r");
            msleep(100);
            read(fd, buff, buffSize);
            int deviceId, firmwareVersion;
            if (sscanf(buff, "*V%02d%03d\n", &deviceId, &firmwareVersion) == 2) {
                char deviceName[20];
                deviceIDString(deviceId, deviceName);
                printf("- Device name:\t%s\n", deviceName);
                printf("- Fw version:\tv%d\n", firmwareVersion);
                gotFwVersion = true;
            }
        } else if (!gotSettings) {
            serialPrint(fd, ">T\r");
            msleep(100);
            read(fd, buff, buffSize);
            ShutterStatus shutterStatus;
            bool lightOn;
            int brightness, shutter;
            if (sscanf(buff, "*T%*03d%*03d%*02d%01d%01d%03d\n", &shutter, (int*) &lightOn, &brightness) == 3) {
                shutterStatusString((ShutterStatus) shutter, buff);
                printf("- Shutter:\t%s\n", buff);
                printf("- Light:\t%s\n", lightOn ? "ON" : "OFF");
                printf("- Brightness:\t%d/255\n", brightness);
                gotSettings = true;
            }
        }
        attempts++;
    } while (((!gotFwVersion) || (!gotSettings)) && (attempts < 3));
    if ((!gotFwVersion) || (!gotSettings)) {
        printf("Error: could not read the device configuration!\n");
        return 1;
    }

    while (1) {
        printf("\nAvailable actions:\n");
        printf("  1. Turn on the EL panel\n");
        printf("  2. Turn off the EL panel\n");
        printf("  3. Set the EL panel brightness\n");
        printf("  4. Open the shutter\n");
        printf("  5. Close the shutter\n");
        printf("  6. Tune the shutter position\n");
        printf("  7. Change the shutter speed\n");
        printf("  8. Read settings\n");
        printf("  9. Exit\n");
        printf("Enter a command: > ");
        int command;
        scanf("%d", &command);

        switch (command) {
            case 1: {
                serialPrint(fd, ">L\r");
                break;
            }

            case 2: {
                serialPrint(fd, ">D\r");
                break;
            }

            case 3: {
                printf("Enter the brightness (0-255): > ");
                int brightness;
                scanf("%d", &brightness);
                sprintf(buff, ">B%03d\r", brightness);
                serialPrint(fd, buff);
                break;
            }

            case 4: {
                serialPrint(fd, ">O\r");
                break;
            }

            case 5: {
                serialPrint(fd, ">C\r");
                break;
            }

            case 9: {
                serialClose(fd);
                return 0;
            }

            default: {
                printf("Invalid command!\n");
                break;
            }
        }
    }
}
