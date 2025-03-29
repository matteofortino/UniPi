#include "../internal.h"

namespace kbd {

	void reset()
	{
		outputb(0x60, iCMR);
		outputb(0x61, iRBR);
	}

}
