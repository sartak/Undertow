O_DIR = obj/
O_FILES = $(patsubst %.c,$(O_DIR)%.o, $(sort $(wildcard *.c))) 

undertow: $(O_FILES)
	@rm -f ut
	@gcc -O -o ut $(O_FILES)
	@echo [`date +%T`] Compilation complete.

$(O_DIR)%.o: %.c undertow.h
	@echo [`date +%T`] Compiling $< ...
	@gcc -c -g -Wall -std=c99 -pedantic $< -o $@

clean:
	@rm -f ut core $(O_FILES)
	@echo [`date +%T`] All clean.
