#define _POSIX_C_SOURCE 200809L
#include "screencomposer.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEBUG 1
#include "log.h"

int
main(int argc, char **argv, char **environ)
{
	//	wlr_log_init(WLR_DEBUG, NULL);

	char *startup_cmd = NULL;

	int c;
	while ((c = getopt(argc, argv, ":s:h")) != -1) {
		switch (c) {
		case 's':
			startup_cmd = optarg;
			break;
		default:
			break;
		}
	}
	if (optind < argc) {
		LOG("Usage: %s [-s startup command]\n", argv[0]);
		return 0;
	}

	sc_compositor_init();
	sc_compositor_start();

	setenv("WAYLAND_DISPLAY", sc_compositor_get_socket(), true);

	if (startup_cmd) {
		if (fork() == 0) {
			execl("/bin/sh", "/bin/sh", "-c", startup_cmd, (void *) NULL);
		}
	}
	LOG("socket: %s\n", sc_compositor_get_socket());

	sc_compositor_start_eventloop();

	return 0;
}
