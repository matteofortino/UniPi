#include "../internal.h"

namespace apic {

	void write_rtl(natb irq, natl w)
	{
		out(RTO + irq * 2, w);
	}

}
