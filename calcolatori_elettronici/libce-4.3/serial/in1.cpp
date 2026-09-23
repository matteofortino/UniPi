#include "../internal.h"

namespace serial {

	natb in1()
	{
		natb s;
		do
			s = inputb(iCOM1_LSR);
		while (!(s & 0x01));
		return inputb(iCOM1_RBR);
	}

}
