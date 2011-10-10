#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdio.h>
#include <errno.h>
#include <assert.h>

#ifdef __WIN32__
	#ifndef RELEASE
		#define DBG(args)                                                \
		{                                                                \
			lock_log();                                                  \
			print_error_begin(__FILE__, "", __LINE__);                   \
			print_error args;                                            \
			unlock_log();                                                \
		}
	#else
		#define DBG(args) 
	#endif
#else
	#ifndef RELEASE
		#define DBG(args)                                                \
		{                                                                \
		    lock_log();                                                  \
			print_error_begin(__FILE__, (char*)__func__, __LINE__);      \
			print_error args;                                            \
			unlock_log();                                                \
		}
	#else
		#define DBG(args) 
	#endif
#endif



#define PRINT_MSG	printf
void print_error_begin(char* file, char* function, int line);
void print_error(const char *fmt, ...);
void hex_dump( unsigned char * buf, int len );
void debug_term_on();
void debug_term_off();
void debug_file_on();
void debug_file_off();
void debug_set_dir(char* str);
void lock_log();
void unlock_log();

#endif //_DEBUG_H

