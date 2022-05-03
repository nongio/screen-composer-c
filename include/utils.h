#ifndef _SC_UTILS_FILE_H
#define _SC_UTILS_FILE_H

#include <time.h>

const char *sc_read_file(const char *filename);

double linspace(int steps, int step);

double interpolate(double p1, double p2, double f);

double bezier_easing_function(double x1, double y1, double x2, double y2,
							  double x_fraction);

double timespec_to_double(struct timespec ts);

#endif
