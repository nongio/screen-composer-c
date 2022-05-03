// credits https://github.com/solemnwarning/timespec/blob/master/timespec.c
#include "utils.h"

#define NSEC_PER_SEC 1000000000


/** \fn double timespec_to_double(struct timespec ts)
 *  \brief Converts a timespec to a fractional number of seconds.
*/
double timespec_to_double(struct timespec ts)
{
	return ((double)(ts.tv_sec) + ((double)(ts.tv_nsec) / NSEC_PER_SEC));
}