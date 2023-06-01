#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "msleep.h"
#include "serial.h"
#include "alnitak.h"
#include "printf_colors.h"

bool readSettings(int fd, int deviceId);
void waitForMotorStop(int fd);
