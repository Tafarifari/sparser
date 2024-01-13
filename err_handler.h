#ifndef __ERROR_HANDLER_H
	#define __ERROR_HANDLER_H

	#include <unistd.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <errno.h>

	#define CHECK(X) ({ int __val = (X);\
						(__val == -1 ? \
							({ fprintf(stderr, "ERROR (" __FILE__ " %d) -- %s\n", __LINE__, strerror(errno)); \
							   exit(-1); -1; }) \
							: __val ); })

	#define CHECK_ALLOC(X) ({ void * __val = (void *) (X);\
							(__val == NULL ? \
								({ fprintf(stderr, "ERROR ( " __FILE__ " : %d ) -- allocation failed\n",  __LINE__); \
								   exit(-1); NULL; }) \
								: __val ); })

#endif