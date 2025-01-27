# Compiler and flags
CC = gcc
CFLAGS = -I/opt/homebrew/include/SDL2 -Iinclude
LDFLAGS = -L/opt/homebrew/lib
LDLIBS = -lsdl2 -lsdl2_image -lsdl2_ttf -lsdl2_mixer

# Source files
SOURCES = $(wildcard src/*.c)

# Binary name
TARGET = typing_warrior

# Default target
all: $(TARGET)

# Build and link
$(TARGET): $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $@
	@echo "\nBuild successful! Run './typing_warrior' to play the game."

# Clean build files
clean:
	rm -f $(TARGET)
	@echo "Cleaned build files."

# Install dependencies 
install-deps:
	brew install sdl2 sdl2_image sdl2_ttf sdl2_mixer

# Phony targets
.PHONY: all clean install-deps