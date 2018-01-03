# 
# Makefile for project 1 
#
# Simulation of swapping processes into memory
# and scheduling them on a CPU
#


## CC  = Compiler.
## CFLAGS = Compiler flags.
CC	= gcc
CFLAGS =	-Wall -Wextra -std=gnu99 -g


## OBJ = Object files.
## SRC = Source files.
## EXE = Executable name.

SRC =		swap.c list.c functions.c
OBJ =		swap.o list.o functions.o
EXE = 		swap

## Top level target is executable.
$(EXE):	$(OBJ)
		$(CC) $(CFLAGS) -o $(EXE) $(OBJ) -lm


## Clean: Remove object files and core dump files.
clean:
		/bin/rm $(OBJ) 

## Clobber: Performs Clean and removes executable file.

clobber: clean
		/bin/rm $(EXE) 

## Dependencies

list.o:	list.h functions.h
functions.o:list.h
