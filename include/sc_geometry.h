#ifndef _SC_GEOMETRY_H
#define _SC_GEOMETRY_H

#include <stdbool.h>
#include <stdio.h>

struct sc_point {
	double x;
	double y;
};

struct sc_rect {
	double x;
	double y;
	double width;
	double height;
};

#endif

