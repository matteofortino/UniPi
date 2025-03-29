#include "../internal.h"

namespace serial {

	void out2(char c)
	{
		natb s;
		do {
			s = inputb(iCOM2_LSR);
		} while (! (s & 0x20));
		outputb(c, iCOM2_THR);
	}

}
