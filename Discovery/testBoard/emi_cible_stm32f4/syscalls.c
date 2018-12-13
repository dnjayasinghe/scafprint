//@Author: Zakaria Najm
//@Brief:Some redefintions of low level hardware specific functions 
//_sbrk is needed for heap allocations (malloc,calloc,free)
//_read && _write are used by stdio fuctions such as printf, fgets, fwrite etc.
//malloc is not yet redefined and the default algorithm from C library is used.
//This fuction may be buggy in some circonstances, and execution time is highly
//variable.

#include <sys/stat.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "usbd_cdc_vcp.h"

int __errno;

int _close(int file) {
	return -1;
}
int _fstat(int file, struct stat *st) {
	return 0;
}
char *__env[1] = { 0 };
char **environ = __env;

int execve(char *name, char **argv, char **env) {
  __errno = -1;
  return -1;
}

int _isatty(int file) {
	return 1;
}

int _lseek(int file, int ptr, int dir) {
	return 0;
}

int _open(const char *name, int flags, int mode) {
	return -1;
}

int _kill(const char *name, int flags, int mode) {

	return -1;

}
int _get_pid(const char *name, int flags, int mode) {

	return 1;
}
int _getpid(const char *name, int flags, int mode) {
	return -1;
}
	
	int _read(int file, char *ptr, int len) {
	if (file != 0) {
		return 0;
	}

	// Use USB CDC Port for stdin
	while(!VCP_get_char((uint8_t*)ptr)){};

	// Echo typed characters
	VCP_put_char((uint8_t)*ptr);

	return 1;
}

/* Register name faking - works in collusion with the linker.  */
register char * stack_ptr asm ("sp");

caddr_t _sbrk_r (struct _reent *r, int incr) {
	extern char   end asm ("end"); /* Defined by the linker.  */
	static char * heap_end;
	char *        prev_heap_end;

	if (heap_end == NULL)
		heap_end = & end;

	prev_heap_end = heap_end;

	if (heap_end + incr > stack_ptr) {
		//errno = ENOMEM;
		return (caddr_t) -1;
	}

	heap_end += incr;

	return (caddr_t) prev_heap_end;
}

int _write(int file, char *ptr, int len) {
	VCP_send_buffer((uint8_t*)ptr, len);
	return len;
}
