#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

double
linspace(int steps, int step)
{
	return (double) step / (double) (steps - 1);
}

double
interpolate(double p1, double p2, double f)
{
	return p1 * (1 - f) + p2 * f;
}

// easing version of the bezier 1d with p0 = 0 and p3 = 1
double
bezier_easing_1d(double p1, double p2, double f)
{
	double f2 = f * f;
	double f3 = f2 * f;
	return f3 + 3 * f3 * p1 - 3 * f3 * p2 + 3 * f2 * p2 - 6 * f2 * p1 +
		   3 * f * p1;
}

// derivative of the easing version of the bezier 1d with p0 = 0 and p3 = 1
double
bezier_easing_1d_prime(double p1, double p2, double f)
{
	double f2 = f * f;
	return 3 * f2 + 9 * f2 * p1 - 9 * f2 * p2 + 6 * f * p2 - 12 * f * p1 +
		   3 * p1;
}

// newthon method to find the roots
double
find_root(double p1, double p2, double target)
{
	double p0 = 0.5;
	double tolerance = 1e-9;
	double epsilon = 1e-14;
	int max_iter = 100;
	for (int ii = 0; ii < max_iter; ++ii) {
		double y = bezier_easing_1d(p1, p2, p0) - target;
		double y_prime = bezier_easing_1d_prime(p1, p2, p0);
		if (fabs(y_prime) < epsilon) {
			break;
		}
		double p_next = p0 - y / y_prime;
		if (fabs(p_next - p0) <= tolerance) {
			return p_next;
		}
		p0 = p_next;
	}
	// numerical difficulties
	return NAN;
}

double
bezier_easing_function(double x1, double y1, double x2, double y2,
					   double x_fraction)
{
	assert(x1 >= 0.0 && x1 <= 1.0);
	assert(x1 >= 0.0 && x1 <= 1.0);
	assert(x_fraction >= 0.0 && x_fraction <= 1.0);
	double curve_fraction = find_root(x1, x2, x_fraction);
	return bezier_easing_1d(y1, y2, curve_fraction);
}
