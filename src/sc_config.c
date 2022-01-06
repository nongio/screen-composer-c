#define _POSIX_C_SOURCE 200809L
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "sc_config.h"
#include "log.h"

struct sc_configuration configuration;

static int handler(void* user, const char* section, const char* name,
                   const char* value)
{
    struct sc_configuration* pconfig = (struct sc_configuration*)user;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
	//LOG("[%s] %s: '%s' %d\n", section, name, value, atoi(value));
    if (MATCH("Display", "resolution_width")) {
        pconfig->display_width = atoi(value);
    } else if (MATCH("Display", "resolution_height")) {
        pconfig->display_height = atoi(value);
    } else if (MATCH("Display", "resolution_refresh")) {
        pconfig->display_refresh = atoi(value);
    } else if (MATCH("Display", "max_render_time")) {
        pconfig->max_render_time = atoi(value);
    } else if (MATCH("Compositor", "shaders_path")) {
        pconfig->shaders_path = strdup(value);
    } else {
        return 0;  /* unknown section/name, error */
    }
    return 1;
}

bool sc_load_config(const char * path)
{
    if (ini_parse(path, handler, &configuration) < 0) {
        LOG("Can't load %s\n", path);
        return true;
    }
	return false;
}

