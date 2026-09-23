#include "../internal.h"

namespace apic {

	natl read_rth(natb irq)
	{
		return in(RTO + irq * 2 + 1);
	}

}
