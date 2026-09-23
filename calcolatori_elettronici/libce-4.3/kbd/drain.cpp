#include "../internal.h"

namespace kbd {

	void drain()
	{
		while (inputb(iSTR) & 0x01) {
			inputb(iRBR);
		}
	}

}
