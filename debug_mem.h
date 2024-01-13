#ifndef __DEBUG_MEM_H
	#define __DEBUG_MEM_H
	
	#include <stdlib.h>

	void f_debug_mem_start(void);
	void f_debug_mem_stop(void);

	void * f_debug_mem_malloc(uint size, char * file, uint line);
	void * f_debug_mem_realloc(void * old_ptr, uint size, char * file, uint line);
	void f_debug_mem_free(void * ptr, char * file, uint line);

	#define malloc(SIZE) 		f_debug_mem_malloc(SIZE, __FILE__, __LINE__)
	#define realloc(P, SIZE)	f_debug_mem_realloc(P, SIZE, __FILE__, __LINE__)
	#define free(P)				f_debug_mem_free(P, __FILE__, __LINE__)

#endif
