#ifndef _SC_CONFIG_H
#define _SC_CONFIG_H

struct sc_configuration {
	int display_width;
	int display_height;
	int display_refresh;
	float display_scale;
	int max_render_time;
	char *shaders_path;
};

bool sc_load_config(const char * path);

#endif
