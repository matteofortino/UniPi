#include "../internal.h"

namespace apic {

	natl apic_read_irr(natb i)
	{
		if (i > 7)
			return 0;
		return *ptr_cast<volatile natl>(0xfee00200 + i * 16);
	}

}
