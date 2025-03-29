#include "../internal.h"

extern "C" bool gate_present(natb num)
{
	gate g = idt[num];
	return g.qw1 & (1ULL << 47);
}
