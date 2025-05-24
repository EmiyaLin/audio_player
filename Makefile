# Makefile for Music Player

# Compiler and compilation flags
CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude
LDFLAGS = -lasound

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Target executable
TARGET_EXEC = music_player
TARGET = $(BIN_DIR)/$(TARGET_EXEC)

# Source files (find all .c files in SRC_DIR)
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Object files (replace src/.../file.c with obj/.../file.o)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

all: $(TARGET)

# Rule to link the target executable from object files
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "Build successful: $(TARGET)"

# Pattern rule to compile .c source files into .o object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to create the object directory if it doesn't exist
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# Target to clean up build artifacts
clean: 
	@echo "Cleaning up build artifacts..."
	@rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Cleanup complete."

.PHONY: all clean