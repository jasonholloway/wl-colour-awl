CC = gcc -Wall -g -O0 -std=gnu99

localSources := $(wildcard *.c)

protos := wlr-ctm-unstable-v1 wlr-gamma-control-unstable-v1
protoSources := $(foreach p,$(protos),build/$(p)-client-protocol.c build/$(p)-client-protocol.h)

build: build/wlcolourawl

build/wlcolourawl: $(localSources) $(protoSources)
	mkdir -p build
	$(CC) -o $@ $^ -lwayland-client

build/%-client-protocol.c: wlroots/protocol/%.xml build/%-client-protocol.h
	mkdir -p build
	wayland-scanner private-code $< $@

build/%-client-protocol.h: wlroots/protocol/%.xml
	mkdir -p build
	wayland-scanner client-header $< $@

buildRun: build/wlcolourawl
	build/wlcolourawl && xxd -c8 dump

clean:
	rm -rf build $(protoSources)

.PHONY: clean buildRun build
