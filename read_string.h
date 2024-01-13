/*
 *	reads string until \n and returns it in allocated array
 *	on EOF and error returns NULL
 */
#ifndef __READ_STRING_H
	#define __READ_STRING_H

	char * read_string(FILE * stream);

#endif