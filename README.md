# Auto Flat AVR

Port of the Auto Flat project in pure C with AVR libraries.

## Description

Auto Flat is PC-controlled lightbox and dust cover implemented using the Alnitak (Flip-Flat/Flat-Man) command set available [here](https://www.optecinc.com/astronomy/catalog/alnitak/resources/Alnitak_GenericCommandsR4.pdf). It is meant to be used with Alnitak-compatible clients or with the included Linux command-line software.

## Serial protocol

```
 - Ping device
    Request: >POOO\r
    Return : *PiiOOO\n
        ii = deviceId

 - Open shutter
    Request: >OOOO\r
    Return : *OiiOOO\n
        ii = deviceId

 - Close shutter
    Request: >COOO\r
    Return : *CiiOOO\n
        ii = deviceId

 - Turn light on
    Request: >LOOO\r
    Return : *LiiOOO\n
        ii = deviceId

 - Turn light off
    Request: >DOOO\r
    Return : *DiiOOO\n
        ii = deviceId

 - Set brightness
    Request: >Bxxx\r
        xxx = brightness value from 000-255
    Return : *Biiyyy\n
        ii  = deviceId
        yyy = value that brightness was set from 000-255

 - Get brightness
    Request: >JOOO\r
    Return : *Jiiyyy\n
        ii = deviceId
        yyy = current brightness value from 000-255

 - Get device status
    Request: >SOOO\r
    Return : *SiiMLC\n
        ii = deviceId
        M  = motor status (0 stopped, 1 running)
        L  = light status (0 off, 1 on)
        C  = Cover Status (0 moving, 1 closed, 2 open, 3 error)

 - Get firmware version
    Request: >VOOO\r
    Return : *Vii001\n
        ii = deviceId

 - Set the servo open value (unofficial command)
    Request: >Qxxx\r
        xxx = servo open value, 0-100
    Return : *Qiiyyy\n
        ii  = deviceId
        yyy = the servo pulse width for the open position

 - Set the servo closed value (unofficial command)
    Request: >Kxxx\r
        xxx = servo closed value, 0-100
    Return : *Kiiyyy\n
        ii  = deviceId
        yyy = the servo pulse width for the closed position

 - Set the servo speed (unofficial command)
    Request: >Zxxx\r
        xxx = servo speed, 0-10
    Return : *Qiiyyy\n
        ii  = deviceId
        yyy = the servo step delay

 - Save settings to EEPROM (unofficial command)
    Request: >YOOO\r
    Return : *YiiOOO\n
        ii = deviceId

 - Get settings (unofficial command)
    Request: >TOOO\r
    Return : *Tuuuvvvxxywzzz\n
        uuu = servo open value, 0-100
        vvv = servo closed value, 0-100
        xx  = servo speed, 0-10
        y   = shutter status, 0 = moving, 1 = closed, 2 = open
        w   = light status, 0-1
        zzz = brightness value, 0-255

```
