# Auto Flat firmware

## Makefile usage

### Options

The following options can be changed or selected in the Makefile:

```
MCU = UNO
MCU = NANO
MCU = NANO_OLD_BOOTLOADER
MCU = MEGA

AVRDUDE_PORT = /dev/ttyACM0
```

### Building the firmware

```
user@computer:~$ make
```

### Uploading

```
user@computer:~$ make flash
```
