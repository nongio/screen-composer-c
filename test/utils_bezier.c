#define _POSIX_C_SOURCE 200809L
#include "utils.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

int
main(int argc, char **argv)
{
	double epsilon = 1e-7;
	assert(fabs(bezier_easing_function(0.0, 1.0, 1.0, 0.0, 0.5) - 0.5) <
		   epsilon);
	assert(fabs(bezier_easing_function(0.0, 1.0, 1.0, 0.0, 0.0) - 0.0) <
		   epsilon);
	assert(fabs(bezier_easing_function(0.0, 1.0, 1.0, 0.0, 1.0) - 1.0) <
		   epsilon);
}
