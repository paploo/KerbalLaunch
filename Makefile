# Setup compile environment.
CC = clang
CFLAGS = -Wall -pedantic -std=c99

RELEASE_CFLAGS = -O3
DEBUG_CFLAGS = -DDEBUG -O0 -g

DEBUGGER = lldb

# Set the name of the products
EXECUTABLE = kerbal_launch
EXECUTABLE_DEBUG = $(EXECUTABLE).debug

# Get the names of the files.
HEADERS = $(wildcard *.h)
SOURCES = $(wildcard *.c)

# Use a replacement rule to get the list of objects.
OBJECTS = $(SOURCES:.c=.o)

# Explicit rules
.PHONY : clean all release debug run rune-debug

# Build
all: release

# Build the exec for release
release: CFLAGS += $(RELEASE_CFLAGS)
release: $(EXECUTABLE)

# Build the exec for debug
debug: CFLAGS += $(DEBUG_CFLAGS)
debug: $(EXECUTABLE_DEBUG)

# The bin is built using the objects.
$(EXECUTABLE): $(OBJECTS)
	$(CC) -v -o $(EXECUTABLE) $(OBJECTS)

$(EXECUTABLE_DEBUG): $(OBJECTS)
	$(CC) -v -o $(EXECUTABLE_DEBUG) $(OBJECTS)

# Make all targets have all headers as dependencies.
# For a project of any size it is better to explicitly list.
$(OBJECTS) : $(HEADERS)

# Rule for building .o files from .c files.
$(OBJECTS) : %.o: %.c
	$(CC) $(CFLAGS) --analyze $<
	$(CC) $(CFLAGS) -c $<

# Run!
run:
	time ./$(EXECUTABLE)

# Run with debugger.
run-debug: debug
	$(DEBUGGER) $(EXECUTABLE_DEBUG)

# Clean!
clean: clean-plists
	rm -rf $(OBJECTS) $(EXECUTABLE) $(EXECUTABLE_DEBUG)

# Remove only the plists.
clean-plists:
	rm -f *.plist
