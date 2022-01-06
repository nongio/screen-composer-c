#ifndef _SC_CONFIG_H
#define _SC_CONFIG_H

#include <ini.h>

struct sc_configuration {
	int display_width;
	int display_height;
	int display_refresh;
	int max_render_time;
	char *shaders_path;
};

bool sc_load_config(const char * path);

#endif
