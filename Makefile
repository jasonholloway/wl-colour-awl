CC = gcc -Wall -g -O0 -std=gnu99

build/ctm: ctm.c
	mkdir -p build
	$(CC) -o $@ ctm.c

.PHONY: clean

clean:
	rm -rf build

