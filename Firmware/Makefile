# MCU
MCU = UNO
#MCU = NANO
#MCU = NANO_OLD_BOOTLOADER
#MCU = MEGA

# Serial port
AVRDUDE_PORT = /dev/ttyACM0


#
# Do not edit below this line
#

# Directories
SRC_DIR = src
BUILD_DIR = build

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Output files
ELF_FILE = $(BUILD_DIR)/main.elf
HEX_FILE = $(BUILD_DIR)/main.hex

# Compiler options
GCC_OPTS = -std=c11 -Wall -Os -DF_CPU=$(F_CPU)

# Avrdude options
AVRDUDE_OPTS = -v -c arduino -U flash:w:$(HEX_FILE) -P $(AVRDUDE_PORT)

# Compiler and Avrdude options based on MCU selection
F_CPU ?= 16000000UL
ifeq ($(MCU), UNO)
    GCC_OPTS += -p=atmega328p
    AVRDUDE_OPTS += -p m328p -b 115200
else ifeq ($(MCU), NANO)
    GCC_OPTS += -p=atmega328p
    AVRDUDE_OPTS += -p m328p -b 115200
else ifeq ($(MCU), NANO_OLD_BOOTLOADER)
    GCC_OPTS += -p=atmega328p
    AVRDUDE_OPTS += -p m328p -b 57600
else ifeq ($(MCU), MEGA)
    GCC_OPTS += -p=atmega2560
    AVRDUDE_OPTS += -p m2560
else
    $(error Unsupported MCU: $(MCU))
endif

# Compile C source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	avr-gcc $(GCC_OPTS) -c $< -o $@

# Link object files
$(ELF_FILE): $(OBJS)
	avr-gcc $(GCC_OPTS) $^ -o $@

# Convert ELF to HEX file
$(HEX_FILE): $(ELF_FILE)
	avr-objcopy -O ihex -R .eeprom $< $@

all:	$(HEX_FILE)

flash:
	avrdude $(AVRDUDE_OPTS)

clean:	
	rm -f build/*

.phony:	clean all flash
