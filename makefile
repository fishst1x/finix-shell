# ===============================
# Description:  makefile for Finix shell program
# ===============================

# ===============================
# Name: Ryan Fisher
# Date: 11-15-2025
# ===============================

CC = gcc
CFLAGS = -Wall

# All source files for the Finix shell
SRCS = finix.c finix_logo.c finix_terminal.c finix_parse.c \
       finix_interactive.c finix_batch.c finix_builtin.c \
       finix_utility.c

# ===============================
# gcc command to compile source code and create finix binary
# Run "make" or "make finix"
# ===============================
finix:
	$(CC) $(CFLAGS) $(SRCS) -o finix

# Default target: same as building finix
all: finix

# ===============================
# Usage information for this makefile
# Run "make usage" to see these instructions
# ===============================
usage:
	@echo "Usage:"
	@echo "  make        # Compile and link the Finix shell (same as 'make finix')"
	@echo "  make finix  # Explicitly build the Finix shell binary"
	@echo "  make clean  # Remove the finix binary"

# ===============================
# Clean up. Run "make clean" to clean directory
# ===============================
clean:
	rm -f finix

.PHONY: all finix clean usage