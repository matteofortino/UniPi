#include "../internal.h"

namespace serial {

	void out1(char c)
	{
		natb s;
		do {
			s = inputb(iCOM1_LSR);
		} while (! (s & 0x20));
		outputb(c, iCOM1_THR);
	}

}
