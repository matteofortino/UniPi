#include "../internal.h"

namespace kbd {

	natb get_code()
	{
		natb c;

		do {
			c = inputb(iSTR);
		} while (!(c & 0x01));
		c = inputb(iRBR);

		return c;
	}

}
