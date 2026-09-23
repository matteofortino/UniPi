#include "../internal.h"

namespace apic {

	natl in(natb off)
	{
		*pIOREGSEL = off;
		return *pIOWIN;
	}

}
