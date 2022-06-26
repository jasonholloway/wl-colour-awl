CC = gcc -Wall -g -O0 -std=gnu99

build/wlcolourawl: wlcolourawl.c
	mkdir -p build
	$(CC) -o $@ wlcolourawl.c

buildRun: build/wlcolourawl
	build/wlcolourawl; xxd -c8 dump

clean:
	rm -rf build

.PHONY: clean
