# Directories
SRC_DIR = src
BUILD_DIR = build

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Output files
OUT_FILE = $(BUILD_DIR)/main

# Compiler options
GCC_OPTS = -std=gnu17 -Wall

.phony: clean all

all: $(OUT_FILE)

# Compile C source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	gcc $(GCC_OPTS) -c $< -o $@

$(OUT_FILE):	$(OBJS) 
	gcc $(GCC_OPTS) -o $@ $^

clean:
	rm -f build/*
