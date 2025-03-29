#include "../internal.h"

namespace kbd {

	void disable_intr()
	{
		outputb(0x60, iCMR);
		outputb(0x60, iTBR);
	}

}
