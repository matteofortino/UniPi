#include "../internal.h"

namespace kbd {

	char char_read()
	{
		natb c;
		do {
			c = get_code();
			if (c == 0x2A)			// make code dello shift destro
				shift = true;
			else if (c == 0xAA)
				shift = false;		// break code dello shift destro
		} while (c >= 0x80 || c == 0x2A);
		return conv(c);			// 0 se tasto non riconosciuto
	}

}
