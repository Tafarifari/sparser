#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "read_string.h"

char * 
read_string(FILE * stream)
{
	if(stream == NULL) {
		return NULL;
	}

	int bytes = 0;
	int capacity = 50;
	char c;

	char *buf = (char *) malloc(capacity);
	if(buf == NULL) {
		fprintf(stderr, "read_string: couldn't allocate memory for string buffer\n");
		return NULL;
	}

	while((c = fgetc(stream)) != EOF && c != '\r' && c != '\n') 
	{
		++bytes;
		if(bytes+1 >= capacity) {
			capacity *= 2;
			buf = (char *) realloc(buf, capacity);
			if(buf == NULL) {
				fprintf(stderr, "read_string: couldn't reallocate memory for string buffer to %d\n", capacity);
				return NULL;
			}
		}

		buf[bytes-1] = c;
	}

	if(c == '\r') {
		c = fgetc(stream);	
	}
	if(c != '\n') {
		ungetc(c, stream);
	}	

	if(bytes == 0) {
		if(c == EOF) {
			return NULL;
		}
	}

	buf[bytes] = 0;

	return buf;
}