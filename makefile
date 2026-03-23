# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra

# Program names (no paths)
PROGS = bmh bmo kmp kra dmb tgen times

# Derived paths
BIN = $(PROGS:%=bin/%)
OBJ = $(PROGS:%=obj/%.o)

# Default target
all: $(BIN)

# Link step
bin/%: obj/%.o
	$(CC) $< -o $@

# Compile step
obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -f bin/* obj/*