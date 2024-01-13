#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>


const char * debug_mem_filename = "debug_mem.log";
FILE * debug_mem_file_ptr = NULL;


void f_debug_mem_start(void)
{
	// return if file already opened
	if(debug_mem_file_ptr) return;

	debug_mem_file_ptr = fopen(debug_mem_filename, "w");
	if(debug_mem_file_ptr == NULL) {
		fprintf(stderr, "f_debug_mem_start: fopen() failed\n");
		exit(-1);
	}
}

void f_debug_mem_stop(void)
{
	fclose(debug_mem_file_ptr);
}


void * f_debug_mem_malloc(uint size, char * file, uint line)
{
	if(debug_mem_file_ptr == NULL) 
	{
		debug_mem_file_ptr = fopen(debug_mem_filename, "w");
		if(debug_mem_file_ptr == NULL) 
		{
			fprintf(stderr, "f_debug_mem_malloc: fopen() failed\n");
			exit(-1);
		}
	}
	void * ptr = malloc(size);
	fprintf(debug_mem_file_ptr, "%s\tmalloc\t%s (%d)\t%d\t[%p]\n", __TIME__, file, line, size, ptr);
	return ptr;
}

void * f_debug_mem_realloc(void * old_ptr, uint size, char * file, uint line)
{
	if(debug_mem_file_ptr == NULL)
	{
		debug_mem_file_ptr = fopen(debug_mem_filename, "w");
		if(debug_mem_file_ptr == NULL) 
		{
			fprintf(stderr, "f_debug_mem_realloc: fopen() failed\n");
			exit(-1);
		}
	}
	void * new_ptr = realloc(old_ptr, size);
	fprintf(debug_mem_file_ptr, "%s\trealloc\t%s (%d)\t%d\t[%p]\t[%p]\n", __TIME__, file, line, size, old_ptr, new_ptr);
	return new_ptr;
}

void f_debug_mem_free(void * ptr, char * file, uint line)
{
	if(debug_mem_file_ptr == NULL)
	{
		debug_mem_file_ptr = fopen(debug_mem_filename, "w");
		if(debug_mem_file_ptr == NULL) 
		{
			fprintf(stderr, "f_debug_mem_free: fopen() failed\n");
			exit(-1);
		}
	}
	free(ptr);
	fprintf(debug_mem_file_ptr, "%s\tfree\t%s (%d)\t\t[%p]\n", __TIME__, file, line, ptr);
}
