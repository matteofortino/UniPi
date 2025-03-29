#include "../internal.h"

namespace apic {

	void write_rth(natb irq, natl w)
	{
		out(RTO + irq * 2 + 1, w);
	}

}
