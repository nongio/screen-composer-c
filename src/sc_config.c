#include <stdbool.h>
#include <stdlib.h>
#include "sc_config.h"
#include "log.h"

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
    } else {
        return 0;  /* unknown section/name, error */
    }
    return 1;
}

bool sc_load_config(const char * path, struct sc_configuration * configuration)
{
    if (ini_parse(path, handler, configuration) < 0) {
        LOG("Can't load %s\n", path);
        return true;
    }
	return false;
}

