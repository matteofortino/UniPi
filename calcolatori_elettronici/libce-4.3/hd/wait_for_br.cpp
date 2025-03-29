#include "../internal.h"

namespace hd {

	void wait_for_br()
	{
		natb s;
		do
			s = inputb(iSTS);
		while ((s & 0x88) != 0x08);
	}

}
