# Compiler to use
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra

# Target executable name
TARGET = shell

# Default target
all: $(TARGET)

# Compile the program
$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

# Clean up built files
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: all clean