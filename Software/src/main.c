#include "main.h"

int main(int argc, char** argv) {
    printf(RED);
    printf("    ___         __           ________      __ \n");
    printf("   /   | __  __/ /_____     / ____/ /___ _/ /_\n");
    printf("  / /| |/ / / / __/ __ \\   / /_  / / __ `/ __/\n");
    printf(" / ___ / /_/ / /_/ /_/ /  / __/ / / /_/ / /_  \n");
    printf("/_/  |_\\____/\\__/\\____/  /_/   /_/\\____/\\__/  \n");
    printf("\t\tcommand-line utility\n\n" NO_COLOR);
                                              

    if (argc < 2) {
        printf(MAGENTA "Usage: %s /dev/ttyUSBx\n" NO_COLOR, argv[0]);
        return 1;
    }

    char* serialPort = argv[1];
    printf(GREEN "Opening serial port %s... ", serialPort);
    int fd = serialOpen(serialPort, 9600);
    if ((fd > 0) && serialSetBlocking(fd, 1)) {
        printf("connected.\n\n" NO_COLOR);
    } else {
        printf(RED "error opening the serial port!\n" NO_COLOR);
        return 1;
    }
    msleep(100);

    printf(GREEN "Device info:\n" NO_COLOR);
    volatile bool gotFwVersion = false, gotSettings = false;
    int attempts = 0;
    const int buffSize = 20;
    char buff[buffSize];
    int deviceId, firmwareVersion;
    do {
        if (!gotFwVersion) {
            serialPrint(fd, ">V\r");
            msleep(100);
            read(fd, buff, buffSize);
            if (sscanf(buff, "*V%02d%03d\n", &deviceId, &firmwareVersion) == 2) {
                char deviceName[20];
                deviceIDString(deviceId, deviceName);
                printf("  Device name:\t%s\n", deviceName);
                printf("  Fw version:\tv%d\n", firmwareVersion);
                gotFwVersion = true;
            }
        } else if (!gotSettings) {
            gotSettings = readSettings(fd, deviceId);
        }
        attempts++;
    } while (((!gotFwVersion) || (!gotSettings)) && (attempts < 3));
    if ((!gotFwVersion) || (!gotSettings)) {
        printf(RED "Error: could not read the device configuration!\n" NO_COLOR);
        return 1;
    }

    printf("\n");
    while (1) {
        printf(GREEN "Available actions:\n" NO_COLOR);
        printf("  1. Turn on the EL panel\n");
        printf("  2. Turn off the EL panel\n");
        printf("  3. Set the EL panel brightness\n");
        printf("  4. Open the shutter\n");
        printf("  5. Close the shutter\n");
        printf("  6. Tune the shutter position\n");
        printf("  7. Change the shutter speed\n");
        printf("  8. Read settings\n");
        printf("  9. Exit\n");
        printf(YELLOW "Enter a command: > " NO_COLOR);
        int command;
        if (scanf("%d", &command) != 1) {
            printf(RED "Invalid command!\n\n" NO_COLOR);
            flushStdin();
            continue;
        }
        flushStdin();

        switch (command) {
            case 1: {
                serialPrint(fd, ">L\r");
                printf(GREEN "Done.\n\n" NO_COLOR);
                break;
            }

            case 2: {
                serialPrint(fd, ">D\r");
                printf(GREEN "Done.\n\n" NO_COLOR);
                break;
            }

            case 3: {
                printf(YELLOW "Enter the brightness (0-255): > " NO_COLOR);
                int brightness;
                if (scanf("%d", &brightness) == 1) {
                    sprintf(buff, ">B%03d\r", brightness);
                    serialPrint(fd, buff);
                    printf(GREEN "Done.\n\n" NO_COLOR);
                } else {
                    printf(RED "Invalid brightness!\n\n" NO_COLOR);
                }
                flushStdin();
                break;
            }

            case 4: {
                if (deviceId != FLIP_FLAT) {
                    printf(RED "The shutter is only available on Flip-Flats.\n\n" NO_COLOR);
                    break;
                }
                printf("Opening");
                serialPrint(fd, ">O\r");
                waitForMotorStop(fd);
                printf(GREEN "Done.\n\n" NO_COLOR);
                break;
            }

            case 5: {
                if (deviceId != FLIP_FLAT) {
                    printf(RED "The shutter is only available on Flip-Flats.\n\n" NO_COLOR);
                    break;
                }
                printf("Closing");
                serialPrint(fd, ">C\r");
                waitForMotorStop(fd);
                printf(GREEN "Done.\n\n" NO_COLOR);
                break;
            }

            case 6: {
                if (deviceId != FLIP_FLAT) {
                    printf(RED "The shutter is only available on Flip-Flats.\n\n" NO_COLOR);
                    break;
                }
                printf("Which position would you like to tune?\n");
                printf("  1. Open position\n");
                printf("  2. Closed position\n");
                printf(YELLOW "  > " NO_COLOR);
                int posToTune;
                if (scanf("%d", &posToTune) != 1) {
                    printf(RED "Invalid option!\n\n" NO_COLOR);
                    flushStdin();
                    break;
                }
                flushStdin();
                if (posToTune == 1) {
                    serialPrint(fd, ">O\r");
                    int newPosition;
                    printf(YELLOW "Enter the new open position (0-100) or -1 to exit the tuning menu:\n > " NO_COLOR);
                    while (1) {
                        if (scanf("%d", &newPosition) != 1) {
                            printf(RED "Invalid position!\n\n" NO_COLOR);
                            flushStdin();
                            break;
                        }
                        flushStdin();
                        if ((newPosition >= 0) && (newPosition <= 100)) {
                            sprintf(buff, ">Q%03d\r", newPosition);
                            serialPrint(fd, buff);
                            msleep(100);
                            printf("Open position updated live.\n");
                        } else {
                            break;
                        }
                    }
                    printf(GREEN "Done.\n\n" NO_COLOR);
                } else if (posToTune == 2) {
                    serialPrint(fd, ">C\r");
                    printf(YELLOW "Enter the new closed position (0-100) or -1 to exit the tuning menu:\n  > " NO_COLOR);
                    while (1) {
                        int newPosition;
                        if (scanf("%d", &newPosition) != 1) {
                            printf(RED "Invalid position!\n\n" NO_COLOR);
                            flushStdin();
                            break;
                        }
                        flushStdin();
                        if ((newPosition >= 0) && (newPosition <= 100)) {
                            sprintf(buff, ">K%03d\r", newPosition);
                            serialPrint(fd, buff);
                            msleep(100);
                            printf("Closed position updated live.\n");
                        } else {
                            break;
                        }
                    }
                    printf(GREEN "Done.\n\n" NO_COLOR);
                } else {
                    printf(RED "Invalid option!\n\n" NO_COLOR);
                }
                break;
            }

            case 7: {
                if (deviceId != FLIP_FLAT) {
                    printf(RED "The shutter is only available on Flip-Flats.\n\n" NO_COLOR);
                    break;
                }
                printf(YELLOW "Enter the new speed (0-10) or -1 to go back:\n  > " NO_COLOR);
                int newSpeed;
                if ((scanf("%d", &newSpeed) == 1) && (newSpeed >= 0) && (newSpeed <= 10)) {
                    sprintf(buff, ">Z%02d\r", newSpeed);
                    serialPrint(fd, buff);
                    msleep(100);
                    printf(GREEN "Done.\n\n" NO_COLOR);
                    flushStdin();
                } else {
                    printf(RED "Invalid speed!\n\n" NO_COLOR);
                    flushStdin();
                    break;
                }
                break;
            }

            case 8: {
                printf("Reading settings...\n");
                if (!readSettings(fd, deviceId))
                    printf(RED "Error: could not read the device configuration!\n\n" NO_COLOR);
                else
                    printf(GREEN "Done.\n\n" NO_COLOR);
                break;
            }

            case 9: {
                serialClose(fd);
                printf(GREEN "Bye!\n" NO_COLOR);
                return 0;
            }

            default: {
                printf(RED "Invalid command!\n\n" NO_COLOR);
                break;
            }
        }
    }
}

void flushStdin() {
    int c;
    while (((c = getchar()) != '\n') && (c != EOF)) {}
}

bool readSettings(int fd, int deviceId) {
    const int buffSize = 20;
    char buff[buffSize];
    serialPrint(fd, ">T\r");
    msleep(100);
    read(fd, buff, buffSize);
    bool lightOn;
    int brightness;
    if (deviceId == FLIP_FLAT) {
        int openVal, closedVal, servoSpeed, shutter;
        if (sscanf(buff, "*T%03d%03d%02d%01d%01d%03d\n",
            &openVal, &closedVal, &servoSpeed, &shutter, (int*) &lightOn, &brightness) == 6) {
            printf("  Open pos.:\t%d\n", openVal);
            printf("  Closed pos.:\t%d\n", closedVal);
            printf("  Speed:\t%d\n", servoSpeed);
            shutterStatusString((ShutterStatus) shutter, buff);
            printf("  Shutter:\t%s\n", buff);
            printf("  Light:\t%s\n", lightOn ? "ON" : "OFF");
            printf("  Brightness:\t%d/255\n", brightness);
            return true;
        }
    } else {
        if (sscanf(buff, "*T%*03d%*03d%*02d%*01d%01d%03d\n", (int*) &lightOn, &brightness) == 2) {
            printf("  Light:\t%s\n", lightOn ? "ON" : "OFF");
            printf("  Brightness:\t%d/255\n", brightness);
            return true;
        }
    }
    return false;
}

void waitForMotorStop(int fd) {
    const int buffSize = 20;
    char buff[buffSize];
    int attempts = 0;
    do {
        msleep(400);
        printf(".");
        fflush(stdout);
        serialPrint(fd, ">S\r");
        msleep(100);
        read(fd, buff, buffSize);
        int shutter;
        if (sscanf(buff, "*S%*02d%*01d%*01d%01d\n", &shutter) == 1) {
            if (((ShutterStatus) shutter) != MOVING) {
                printf("\n");
                return;
            }
        }
        attempts++;
    } while (attempts < 90);
    printf(RED "\nError: motor timerout.\n" NO_COLOR);
}
