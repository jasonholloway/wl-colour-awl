#define _POSIX_C_SOURCE 200809L                 
#define _GNU_SOURCE
#include <errno.h>                              
#include <stdio.h>                              
#include <stdlib.h>                             
#include <string.h>                             
#include <sys/mman.h>                           
#include <sys/types.h>                          
#include <unistd.h>                             
#include <wayland-client-protocol.h>            
#include <wayland-client.h>                     
#include "build/wlr-ctm-unstable-v1-client-protocol.h"

struct output {
	struct wl_output *wl_output;
	struct zwlr_ctm_control_v1 *control;
	struct wl_list link;
};

static struct wl_list outputs;
static struct zwlr_ctm_manager_v1 *manager = NULL;

static int write_file(off_t size, void *data) {
  int fd = memfd_create("relay", 0);
	if (fd < 0) {
		return -1;
	}

  int c = write(fd, data, size);
  if(c < size) {
		close(fd);
    return -1;
  }

	return fd;
}


static void ctm_control_handle_failed(void *data, struct zwlr_ctm_control_v1 *control) {
	fprintf(stderr, "failed to set ctm\n");
	exit(EXIT_FAILURE);
}

static const struct zwlr_ctm_control_v1_listener control_listener = {
	.failed = ctm_control_handle_failed,
};



static void registry_handle_global(void *data, struct wl_registry *registry,
		uint32_t name, const char *interface, uint32_t version) {
	if (strcmp(interface, wl_output_interface.name) == 0) {
		struct output *output = calloc(1, sizeof(struct output));
		output->wl_output = wl_registry_bind(registry, name, &wl_output_interface, 1);
		wl_list_insert(&outputs, &output->link);
	} else if (strcmp(interface, zwlr_ctm_manager_v1_interface.name) == 0) {
		manager = wl_registry_bind(registry, name, &zwlr_ctm_manager_v1_interface, 1);
	}
}

static void registry_handle_global_remove(void *data,
		struct wl_registry *registry, uint32_t name) {
	// Who cares?
}

static const struct wl_registry_listener registry_listener = {
	.global = registry_handle_global,
	.global_remove = registry_handle_global_remove,
};

int relayCtm(const char *displayName, int64_t *ctm) {
  FILE *dumpFile = fopen("dump", "w");
  fwrite(ctm, sizeof(int64_t), 9, dumpFile);
  fclose(dumpFile);

	struct wl_display *display = wl_display_connect(displayName);
	if (display == NULL) {
		fprintf(stderr, "failed to connect to %s\n", displayName);
		return -1;
	}

	wl_list_init(&outputs);

	struct wl_registry *registry = wl_display_get_registry(display);
	wl_registry_add_listener(registry, &registry_listener, NULL);
	wl_display_roundtrip(display);

	if (manager == NULL) {
		fprintf(stderr,
			"compositor doesn't support wlr-ctm-unstable-v1\n");
		return EXIT_FAILURE;
	}

	struct output *output;
	wl_list_for_each(output, &outputs, link) {
		output->control = zwlr_ctm_manager_v1_get_ctm_control(manager, output->wl_output);
		zwlr_ctm_control_v1_add_listener(output->control, &control_listener, output);
	}
	wl_display_roundtrip(display);

  int fd = write_file(sizeof(int64_t) * 9, ctm);
  if(fd < 0) {
    printf("error creating file");
    return 1;
  }

	wl_list_for_each(output, &outputs, link) {
		zwlr_ctm_control_v1_set_ctm(output->control, fd);
	}

	fprintf(stderr, "Dispatching...\n");

	while (wl_display_dispatch(display) != -1) {
		// This space is intentionally left blank
	}

  return 0;
}
