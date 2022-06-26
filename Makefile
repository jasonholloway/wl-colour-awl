CC = gcc -Wall -g -O0 -std=gnu99

localSources := $(wildcard *.c)

protos := wlr-ctm-unstable-v1 wlr-gamma-control-unstable-v1
protoSources := $(foreach p,$(protos),$(p)-client-protocol.c $(p)-client-protocol.h)

build: build/wlcolourawl

build/wlcolourawl: $(localSources) $(protoSources)
	mkdir -p build
	$(CC) -o $@ $^

%-client-protocol.c: wlroots/protocol/%.xml
	wayland-scanner private-code $< $@

%-client-protocol.h: wlroots/protocol/%.xml
	wayland-scanner client-header $< $@

buildRun: build/wlcolourawl
	build/wlcolourawl; xxd -c8 dump

clean:
	rm -rf build $(protoSources)

.PHONY: clean buildRun build
