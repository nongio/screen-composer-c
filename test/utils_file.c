#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdlib.h>

#include "log.h"
#include "utils.h"

const char * file_content =
"hello\n"
"this\n"
"is\n"
"an\n"
"example\n"
"file\n"
"\n"
"\n";
int
main(int argc, char **argv)
{
	const char *filename = argv[1];
	const char *testfile = sc_read_file(filename);
	assert(testfile != NULL);
	assert(strcmp(file_content, testfile) == 0);
	return 0;
}
