#include "../internal.h"

namespace kbd {

	char char_read_intr()
	{
		natb c = inputb(iRBR);
		if (c == 0x2A)
			shift = true;
		else if (c == 0xAA)
			shift = false;
		if (c >= 0x80 || c == 0x2A)
			return 0;
		return conv(c);
	}

}
