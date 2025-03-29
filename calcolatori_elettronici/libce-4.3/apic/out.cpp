#include "../internal.h"

namespace apic {

	void out(natb off, natl v)
	{
		*pIOREGSEL = off;
		*pIOWIN = v;
	}

}
