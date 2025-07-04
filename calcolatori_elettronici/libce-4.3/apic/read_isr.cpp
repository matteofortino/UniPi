#include "../internal.h"

namespace apic {

	natl apic_read_isr(natb i)
	{
		if (i > 7)
			return 0;
		return *ptr_cast<volatile natl>(0xfee00100 + i * 16);
	}

}
