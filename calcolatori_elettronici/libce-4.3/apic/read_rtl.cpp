#include "../internal.h"

namespace apic {

	natl read_rtl(natb irq)
	{
		return in(RTO + irq * 2);
	}

}
