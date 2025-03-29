#include "../internal.h"

namespace serial {

	natb in1()
	{
		natb s;
		do
			s = inputb(iCOM2_LSR);
		while (!(s & 0x01));
		return inputb(iCOM2_RBR);
	}

}
