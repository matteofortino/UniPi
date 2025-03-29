#include "../internal.h"

namespace kbd {

	void enable_intr()
	{
		outputb(0x60, iCMR);
		outputb(0x61, iTBR);
	}

}
