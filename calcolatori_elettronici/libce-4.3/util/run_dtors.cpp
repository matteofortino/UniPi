#include "../internal.h"

extern "C" void run_dtors()
{
	for (next_exit_func--; next_exit_func >= 0; next_exit_func--) {
		exit_rec *e = &exit_funcs[next_exit_func];
		e->fun(e->arg);
	}
}
