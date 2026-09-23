#include "../internal.h"


extern "C" void gate_init(natb num, void routine(), bool trap, int liv)
{
	idt[num] = componi_gate(routine, trap, liv);
}
