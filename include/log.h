#ifndef _SC_LOG_H
#define _SC_LOG_H

#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#define LOG(...) fprintf(stdout, __VA_ARGS__)
#define ELOG(...)  fprintf(stderr, __VA_ARGS__)
#ifdef DEBUG
#define DLOG(...) fprintf(stdout, __VA_ARGS__)
#else
#define DLOG(...) ()
#endif
#define LOG_ERRNO(...)  fprintf(stderr, "Error : %s\n", strerror(errno)); fprintf(stderr, __VA_ARGS__)

#endif
