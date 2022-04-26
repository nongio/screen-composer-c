inline double
interpolate(double p1, double p2, double f)
{
	return p1 * (1 - f) + p2 * f;
}

inline double
bezier_1d(double p0, double p1, double p2, double p3, double f)
{
	double p1p2 = interpolate(p1, p2, f);
	return interpolate(interpolate(interpolate(p0, p1, f), p1p2, f),
					   interpolate(p1p2, interpolate(p2, p3, f), f), f);
}

void
bezier_2d(double x0, double y0, double x1, double y1, double x2, double y2,
		  double x3, double y3, float f, double *x, double *y)
{
	*x = bezier_1d(x0, x1, x2, x3, f);
	*y = bezier_1d(y0, y1, y2, y3, f);
}

double
linspace(int steps, int step)
{
	return (double) step / (double) (steps - 1);
}
