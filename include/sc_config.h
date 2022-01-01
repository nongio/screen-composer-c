#ifndef _SC_CONFIG_H
#define _SC_CONFIG_H

#include <ini.h>

struct sc_configuration {
	int display_width;
	int display_height;
	int display_refresh;
};

bool sc_load_config(const char * path, struct sc_configuration * config);

#endif
