#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "../../include/tcpsum.h"

// argv[1] = filename
int main(int argc, char **argv)
{
	char *filename;
	struct stat st;
	unsigned int size;
	FILE *fp;
	unsigned char *buf;
	int got;

	if (argc < 2) {
		fprintf(stderr, "missed arg1: filename\n");
		return -1;
	}

	filename = argv[1];

	// get file size
	memset(&st, 0, sizeof(st));
	stat(filename, &st);
	size = st.st_size;

	if (size == 0) {
		fprintf(stderr, "%s is empty file\n", filename);
		return -1;
	}

	buf = malloc(size);
	if (!buf) {
		fprintf(stderr, "can't malloc size %u\n", size);
		return -1;
	}
	

	fp = fopen(filename, "rb");
	got = fread(buf, 1, size, fp);
	if (got != size) {
		fprintf(stderr, "only read %d bytes (total=%d)\n", got, size);
		return -1;
	}

	
	// hex
	printf("%04x\n", tcpsum(buf, size));

	free(buf);
	return 0;
}
