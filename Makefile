CC      := gcc
CFLAGS  := -Wall -Wextra -Iinc -g
LDFLAGS := -lpthread

SRC_DIR := src
INC_DIR := inc
BUILD_DIR := build
TARGET := app

# Find all .c files in src/
SRCS := $(wildcard $(SRC_DIR)/*.c)

# Default target
all: $(BUILD_DIR) $(BUILD_DIR)/$(TARGET)

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile all sources directly into executable
$(BUILD_DIR)/$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
