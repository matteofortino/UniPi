#include "../internal.h"

extern "C" void __cxa_atexit(void (*f)(void *), void *a, void *)
{
	if (next_exit_func >= MAX_EXIT_FUNCS) {
		panic("too many destructors");
	}
	exit_rec *e = &exit_funcs[next_exit_func];
	e->fun = f;
	e->arg = a;
	next_exit_func++;
}
