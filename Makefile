# Setup compile environment.
CC = clang
CFLAGS = -Wall -pedantic -std=c99 -O3

# Set the name of the products
BIN = kerbal_launch

# Get the names of the files.
HEADERS = $(wildcard *.h)
SOURCES = $(wildcard *.c)

# Use a replacement rule to get the list of objects.
OBJECTS = $(SOURCES:.c=.o)

# Explicit rules
.PHONY : clean all build run clean-plists

# Build (and run if we want)
all: build run

# Build the bin and clean plists.
build: $(BIN) clean-plists

# The bin is built using the objects.
$(BIN): $(OBJECTS)
	$(CC) -v -o $(BIN) $(OBJECTS)

# Make all targets have all headers as dependencies.
# For a project of any size it is better to explicitly list.
$(OBJECTS) : $(HEADERS)

# Rule for building .o files from .c files.
$(OBJECTS) : %.o: %.c
	$(CC) $(CFLAGS) --analyze $<
	$(CC) $(CFLAGS) -c $<

# Run!
run:
	./$(BIN)

# Clean!
clean:
	rm -rf *.o *.plist $(BIN)

# Remove only the plists.
clean-plists:
	rm -f *.plist
