CFLAGS  = -g -std=c99 -pedantic -Wall
CC      = gcc
S_DIR   = src/
S_FILES = $(sort $(wildcard $(S_DIR)*.c))
O_DIR   = $(S_DIR)obj/
O_FILES = $(patsubst $(S_DIR)%.c,$(O_DIR)%.o, $(S_FILES))

undertow: $(O_FILES)
	@rm -f ut
	@$(CC) -o ut $(O_FILES)
	@echo [`date +%T`] Compilation complete.
	
$(O_DIR)%.o: $(S_DIR)%.c $(S_DIR)undertow.h
	@echo [`date +%T`] Compiling $< ...
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f ut core ut.core $(O_FILES)
	@echo [`date +%T`] All clean.

backup:
	@tar -cjf archive/`date +%Y%m%d`.tar.bz account data help src/*.c
	@echo Created [archive/`date +%Y%m%d`.tar.bz].